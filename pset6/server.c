//
// server.c
//
// Nikhil Jayswal
//
// NOTE: a lot of pointer arithmetic is here to manipulate strings.
// Hence, some code might seem weird. Also, lot of unnecessary code
// and statements are present.
//
// NOTE: free resources before next request

// feature test macro requirements
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html
#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of octets for buffered reads
#define OCTETS 512

// header files
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

// types
typedef char octet;

// prototypes
bool connected(void);
bool error(unsigned short code);
void handler(int signal);
ssize_t load(void);
const char* lookup(const char* extension);
ssize_t parse(void);
void reset(void);
void start(short port, const char* path);
void stop(void);

// my prototypes
// int validate_request(char* line);

// server's root
// string to store server's root directory
char* root = NULL;

// file descriptor for sockets
int cfd = -1, sfd = -1;

// buffer for request - equivalent to char* request
// i.e. a string to store request line
octet* request = NULL;

// FILE pointer for files
FILE* file = NULL;

// buffer for response-body
// string to store response
octet* body = NULL;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;

    // default to a random port
    // initializing variable to 0
    int port = 0;

    // usage
    const char* usage = "Usage: server [-p port] /path/to/root";

    // parse command-line arguments
    // read manual page for getopt() thoroughly
    int opt;
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }

    // ensure port is a non-negative short and path to server's root is specified
    // refer to manual page for getopt()
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL || strlen(argv[optind]) == 0)
    {
        // announce usage
        printf("%s\n", usage);

        // return 2 just like bash's builtins
        return 2;
    }

    // start server
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c)
    signal(SIGINT, handler);

    // accept connections one at a time
    while (true)
    {
        // reset server's state
        reset();

        // wait until client is connected
        if (connected())
        {
            // parse client's HTTP request
            ssize_t octets = parse();
            if (octets == -1)
            {
                continue;
            }

            // extract request's request-line
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
            const char* haystack = request;
            char* needle = strstr(haystack, "\r\n");
            if (needle == NULL)
            {
                error(400);
                continue;
            }
            else if (needle - haystack + 2 > LimitRequestLine)
            {
                error(414);
                continue;
            }   
            char line[needle - haystack + 2 + 1];
            strncpy(line, haystack, needle - haystack + 2);
            line[needle - haystack + 2] = '\0';

            // log request-line
            printf("%s", line);
/*-----------------------------------------------------------------------------
            // TODO: validate request-line
            // TODO: extract query from request-target
-----------------------------------------------------------------------------*/
            // code can be further refined and unnecessary statements deleted
            // but I am too lazy to do that.
            
            // length calculations and string copying might be confusing
            // just think carefully
            // all strings have to be legal terminated by '\0'
            
            // string = [start].....[chr]...['\0']
            // to copy [start] to [chr - 1]
                // length = [chr] - [start] + 1
                // strncpy(new_string, string, length - 1)
                // new_string[length - 1] = '\0'
            // to copy [chr + 1] to ['\0' - 1]
                // length = ['\0'] - [chr + 1] + 1
                // strncpy(new_string, string, length - 1)
                // new_string[length - 1] = '\0'
            
            // get a pointer to line
            char* line_ptr = line;
            
            int value = 200;
            // first check: check format
            // valid format : method SP request‐target SP HTTP‐version CRLF
            
            int length = 0;
            char* method = NULL;
            char* request_target = NULL;
            char* http_version = NULL;
            
            // find first space and get method
            char* space = strchr(line_ptr, ' ');
            if (space != NULL)
            {
                // +1 for the '\0'
                length = space - line_ptr + 1;
                method = malloc(length);
                // -1 because method does not include the SP
                strncpy(method, line_ptr, length - 1);
                method[length - 1] = '\0';
            }
            else
            {
                // for debugging
                printf("No method.\n");
                value = 400;
                error(value);
                continue;
            }
            
            // update line pointer to space + 1
            line_ptr = space + 1;
            // check for more than one space between method and request-target
            if (line_ptr[0] == ' ')
            {
                // for debugging
                printf("More than one space between method and request-target\n");
                value = 400;
                error(value);
                continue; 
            }
            // find next space and get request-target
            space = strchr(line_ptr, ' ');
            if (space != NULL)
            {
                length = space - line_ptr + 1;
                request_target = malloc(length);
                strncpy(request_target, line_ptr, length - 1);
                request_target[length - 1] = '\0';
            }
            else
            {
                // for debugging
                printf("No request_target.\n");
                value = 400;
                error(value);
                continue;
            }
            
            // update line pointer to space + 1
            line_ptr = space + 1;
            // check for more than one space between request-target and HTTP-version
            if (line_ptr[0] == ' ')
            {
                // for debugging
                printf("More than one space between request-target and HTTP-version\n");
                value = 400;
                error(value);
                continue; 
            }
            // check if there are more spaces present in the request-line
            if (strchr(line_ptr, ' ') != NULL)
            {
                // for debugging
                printf("More than two spaces in request-line\n");
                value = 400;
                error(value);
                continue;    
            }
            // find CRLF and get HTTP-version
            space = strstr(line_ptr, "\r\n");
            if (space != NULL)
            {
                length = space - line_ptr + 1;
                http_version = malloc(length);
                strncpy(http_version, line_ptr, length - 1);
                http_version[length - 1] = '\0';
            }
            else
            {
                // for debugging
                printf("No http-version.\n");
                error(value);
                continue;
            }
            // update line pointer to space + 2 (\r\n)
            line_ptr = space + 2;
            // check that there is nothing after CRLF
            if (line_ptr[0] != '\0')
            {
                // for debugging
                printf("Bad Request Ending.\n");
                error(value);
                continue;       
            }
            
            // for debugging
            printf("Method: %s\n",method);
            printf("Request-Target: %s\n", request_target);
            printf("HTTP-version: %s\n", http_version);
           
            // OK, so format of request-line checked
            // Main checks:
            // No more than a total of two spaces
            // No more than one space between the method, request-target and http-version
            // No stuff after CRLF, i.e. request-line ends at CRLF
            
            // second check: request-target = absolute-path["?"query], [] means optional
            // Main checks:
            // absolute-path begins with /
            // absolute path may be followed by a ?
            // ? has to be followed by a query which may not contain "
            
            // check if request-target begins with a /
            if (request_target[0] != '/')
            {
                // for debugging
                printf("Request-target does not begin with /\n");
                value = 501;
                error(value);
                continue;        
            }
            
            // check if request_target contains "
            if (strchr(request_target, '\"') != NULL)
            {
                // for debugging
                printf("Request-target contains \"\n");
                value = 400;
                error(value);
                continue;
            }
            
            // check if request-target contains ?; if yes, then extract query and absolute-path
            char* query_ptr = NULL;
            char* absolute_path = NULL;
            if (strchr(request_target, '?') != NULL)
            {
                // NOTE: string copying is a bit different for query here, because we need to copy
                // the portion after ?; earlier it was portion before SP
                
                space = strchr(request_target, '?');
                // move space forward
                space = space + 1;
                // move request_target pointer to '\0'
                // separate variable used to store length of request_target, because once
                // we move the pointer, the original length is lost
                int len = strlen(request_target);
                request_target = request_target + len;
                length = request_target - space + 1;
                // move request_target pointer back to the start; just subtract what was added
                request_target = request_target - len;
                query_ptr = malloc(length);
                // start copying after ?(pointed to by space) till end of request_target
                strncpy(query_ptr, space, length - 1);
                query_ptr[length - 1] = '\0';
                // for debugging
                printf("Query(1): %s\n", query_ptr);
                        
                // extract absolute-path
                // bring space back to ?
                space = space - 1;
                length = space - request_target + 1;
                absolute_path = malloc(length);
                strncpy(absolute_path, request_target, length - 1);
                absolute_path[length - 1] = '\0';
                // for debugging
                printf("Absolute-path(1): %s\n", absolute_path);
            }
            else
            {
                // else absolute_path is the same as request_target
                length = strlen(request_target) + 1;
                absolute_path = malloc(length);
                strncpy(absolute_path, request_target, length - 1);
                absolute_path[length - 1] = '\0';
                // for debugging
                printf("Absolute-path(2): %s\n", absolute_path);
            }
            
            // third check: method = GET
            if (strcmp(method, "GET") != 0)
            {
                value = 405;
                error(value);
                continue;
            }
            
            // fourth check: HTTP-version = HTTP/1.1
            if (strcmp(http_version, "HTTP/1.1") != 0)
            {
                value = 505;
                error(value);
                continue;
            } 
            
            // fifth check: absolute-path does not contain a .
            if (strchr(absolute_path, '.') == NULL)
            {
                value = 501;
                error(value);
                continue;
            }
                               
/*            char query[] = "abc";*/
/*            strcpy(query, query_ptr);*/
/*            char abs_path[] = "abc";*/
/*            strcpy(abs_path, absolute_path);*/
/*            // for debugging*/
/*            printf("Query(2): %s\n", query);*/
/*            printf("Absolute-Path(3): %s\n", abs_path);        */

            // ALTERNATE CODE
            char* query = query_ptr;
            // for debugging
            printf("Query(2): %s\n", query);            
/*------------------------------------------------------------------------------*/
            // TODO: concatenate root and absolute-path
/*            char path[] = "abc";*/
/*            root = argv[optind];*/
/*            strcat(root, abs_path);*/
/*            strcpy(path, root);*/
/*            // for debugging*/
/*            printf("Path: %s\n", path);*/

            // ALTERNATE CODE
            // root = argv[optind];
            // for debugging            
            printf("Root: %s\n", root);
            char* path = NULL;
            path = malloc(strlen(root) + strlen(absolute_path) + 1);
            // initialize memory to avoid valgrind complaints
            memset(path, '\0', strlen(root) + strlen(absolute_path) + 1);
            strncpy(path, root, strlen(root));
            // for debugging            
            printf("Path(1): %s\n", path);
            char* str = NULL;
            str = path + strlen(path);
            strncpy(str, absolute_path, strlen(absolute_path));
            // set str to NULL to avoid free() problems
            str = NULL;
            path[strlen(root) + strlen(absolute_path)] = '\0';
            // for debugging            
            printf("Path(2): %s\n", path);
/*-----------------------------------------------------------------------------*/
            // TODO: ensure path exists
            // see man page for access
            if (access(path, F_OK) != 0)
            {
                error(404);
                continue;
            }
/*----------------------------------------------------------------------------*/            
            // TODO: ensure path is readable
            // see man page for access
            if (access(path, R_OK) != 0)
            {
                error(403);
                continue;
            }
/*----------------------------------------------------------------------------*/            
             // TODO: extract path's extension
/*            char extension[] = "TODO";*/
/*            // find the last occurrence of . in path*/
/*            space = strrchr(path, '.');*/
/*            // get a pointer to path*/
/*            char* path_ptr = path;*/
/*            // move pointer to end of path, i.e. the null byte*/
/*            path_ptr = path_ptr + strlen(path);*/
/*            length = path_ptr - space + 1;*/
/*            strncpy(extension, path_ptr, length - 1);*/
/*            extension[length] = '\0';*/
/*            // for debugging*/
/*            printf("Extension: %s\n", extension);*/

            // ALTERNATE CODE
            // char* extension = NULL;
            // find the last occurrence of . in path
            space = strrchr(path, '.');
            // move path to null byte at its end
            // see instructions for string copying above at the first TODO
            // to avoid confusion
            int len = strlen(path);
            path = path + len;
            space = space + 1;
            length = path - space + 1;
            char extension[length];
            strncpy(extension, space, length - 1);
            extension[length - 1] = '\0';
            // bring path back to start
            path = path - len;
            // for debugging
            printf("Extension: %s\n", extension);            
            
/*----------------------------------------------------------------------------*/
            // dynamic content
            if (strcasecmp("php", extension) == 0)
            {
                // open pipe to PHP interpreter
                char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi";
                char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
                sprintf(command, format, query, path);
                
                // for debugging
                printf("Filename: %s\n", command);                
                
                file = popen(command, "r");
                if (file == NULL)
                {
                    error(500);
                    continue;
                }

                // load file
                ssize_t size = load();
                if (size == -1)
                {
                    error(500);
                    continue;
                }

                // subtract php-cgi's headers from body's size to get content's length
                haystack = body;
                needle = memmem(haystack, size, "\r\n\r\n", 4);
                if (needle == NULL)
                {
                    error(500);
                    continue;
                }
                size_t length = size - (needle - haystack + 4);

                // respond to client
                if (dprintf(cfd, "HTTP/1.1 200 OK\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Connection: close\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
                {
                    continue;
                }
                if (write(cfd, body, size) == -1)
                {
                    continue;
                }
            }
/*----------------------------------------------------------------------------*/
            // static content
            else
            {
                // look up file's MIME type
                const char* type = lookup(extension);
                if (type == NULL)
                {
                    error(501);
                    continue;
                }

                // open file
                file = fopen(path, "r");
                if (file == NULL)
                {
                    error(500);
                    continue;
                }

                // load file
                ssize_t length = load();
                if (length == -1)
                {
                    error(500);
                    continue;
                }
                // for debugging
                printf("File Length: %i\n", length);

                // TODO: respond to client
                if (dprintf(cfd, "HTTP/1.1 200 OK\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Connection: close\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Content-Type: %s\r\n", type) < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "\r\n") < 0)
                {
                    continue;
                }
                if (write(cfd, body, length) == -1)
                {
                    continue;
                }
            }
            
            // announce OK
            printf("\033[32m");
            printf("HTTP/1.1 200 OK");
            printf("\033[39m\n");
            
            // free memory
            free(method);
            free(request_target);
            free(http_version);
            free(query_ptr); //frees query automatically
            free(absolute_path); 
            free(path); 
                      
            
        }
    
    }
}

/**
 * Accepts a connection from a client, blocking (i.e., waiting) until one is heard.
 * Upon success, returns true; upon failure, returns false.
 */
bool connected(void)
{
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
    if (cfd == -1)
    {
        return false;
    }
    return true;
}

/**
 * Handles client errors (4xx) and server errors (5xx).
 */
bool error(unsigned short code)
{
    // ensure client's socket is open
    if (cfd == -1)
    {
        return false;
    }

    // ensure code is within range
    if (code < 400 || code > 599)
    {
        return false;
    }

    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = NULL;
    switch (code)
    {
        case 400: phrase = "Bad Request"; break;
        case 403: phrase = "Forbidden"; break;
        case 404: phrase = "Not Found"; break;
        case 405: phrase = "Method Not Allowed"; break;
        case 413: phrase = "Request Entity Too Large"; break;
        case 414: phrase = "Request-URI Too Long"; break;
        case 418: phrase = "I'm a teapot"; break;
        case 500: phrase = "Internal Server Error"; break;
        case 501: phrase = "Not Implemented"; break;
        case 505: phrase = "HTTP Version Not Supported"; break;
    }
    if (phrase == NULL)
    {
        return false;
    }

    // template
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s</h1></body></html>";
    char content[strlen(template) + 2 * ((int) log10(code) + 1 - 2) + 2 * (strlen(phrase) - 2) + 1];
    int length = sprintf(content, template, code, phrase, code, phrase);

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)
    {
        return false;
    }

    // respond with Connection header
    if (dprintf(cfd, "Connection: close\r\n") < 0)
    {
        return false;
    }

    // respond with Content-Length header
    if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
    {
        return false;
    }

    // respond with Content-Type header
    if (dprintf(cfd, "Content-Type: text/html\r\n") < 0)
    {
        return false;
    }

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)
    {
        return false;
    }

    // respond with message-body
    if (write(cfd, content, length) == -1)
    {
        return false;
    }

    // announce Response-Line
    printf("\033[31m");
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");

    return true;
}

/**
 * Loads file into message-body.
 */
ssize_t load(void)
{
    // ensure file is open
    if (file == NULL)
    {
        return -1;
    }

    // ensure body isn't already loaded
    if (body != NULL)
    {
        return -1;
    }

    // buffer for octets
    octet buffer[OCTETS];

    // read file
    ssize_t size = 0;
    while (true)
    {
        // try to read a buffer's worth of octets
        ssize_t octets = fread(buffer, sizeof(octet), OCTETS, file);

        // check for error
        if (ferror(file) != 0)
        {
            if (body != NULL)
            {
                free(body);
                body = NULL;
            }
            return -1;
        }

        // if octets were read, append to body
        if (octets > 0)
        {
            body = realloc(body, size + octets);
            if (body == NULL)
            {
                return -1;
            }
            memcpy(body + size, buffer, octets);
            size += octets;
        }

        // check for EOF
        if (feof(file) != 0)
        {
            break;
        }
    }
    return size;
}

/**
 * Handles signals.
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)
    {
        // ensure this isn't considered an error
        // (as might otherwise happen after a recent 404)
        errno = 0;

        // announce stop
        printf("\033[33m");
        printf("Stopping server\n");
        printf("\033[39m");

        // stop server
        stop();
    }
}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* extension)
{
    if (strcasecmp(extension, "css") == 0)
    {
         const char* mime = "text/css";
         return mime;
    }
    else if (strcasecmp(extension, "html") == 0)
    {
         const char* mime = "text/html";
         return mime;
    }
    if (strcasecmp(extension, "gif") == 0)
    {
         const char* mime = "image/gif";
         return mime;
    }
    else if (strcasecmp(extension, "ico") == 0)
    {
         const char* mime = "image/x-icon";
         return mime;
    } 
    else if (strcasecmp(extension, "jpg") == 0)
    {
         const char* mime = "image/jpeg";
         return mime;
    } 
    else if (strcasecmp(extension, "js") == 0)
    {
         const char* mime = "text/javascript";
         return mime;
    } 
    else if (strcasecmp(extension, "png") == 0)
    {
         const char* mime = "image/png";
         return mime;
    }      
    return NULL;
}

/**
 * Parses an HTTP request.
 */
ssize_t parse(void)
{
    // ensure client's socket is open
    if (cfd == -1)
    {
        return -1;
    }

    // ensure request isn't already parsed
    if (request != NULL)
    {
        return -1;
    }

    // buffer for octets
    octet buffer[OCTETS];

    // parse request
    ssize_t length = 0;
    while (true)
    {
        // read from socket
        ssize_t octets = read(cfd, buffer, sizeof(octet) * OCTETS);
        if (octets == -1)
        {
            error(500);
            return -1;
        }

        // if octets have been read, remember new length
        if (octets > 0)
        {
            request = realloc(request, length + octets);
            if (request == NULL)
            {
                return -1;
            }
            memcpy(request + length, buffer, octets);
            length += octets;
        }

        // else if nothing's been read, socket's been closed
        else
        {
            return -1;
        }

        // search for CRLF CRLF
        int offset = (length - octets < 3) ? length - octets : 3;
        char* haystack = request + length - octets - offset;
        char* needle = memmem(haystack, request + length - haystack, "\r\n\r\n", 4);
        if (needle != NULL)
        {
            // trim to one CRLF and null-terminate
            length = needle - request + 2 + 1;
            request = realloc(request, length);
            if (request == NULL)
            {
                return -1;
            }
            request[length - 1] = '\0';
            break;
        }

        // if buffer's full and we still haven't found CRLF CRLF,
        // then request is too large
        if (length - 1 >= LimitRequestLine + LimitRequestFields * LimitRequestFieldSize)
        {
            error(413);
            return -1;
        }
    }
    return length;
}

/**
 * Resets server's state, deallocating any resources.
 */
void reset(void)
{
    // free response's body
    if (body != NULL)
    {
        free(body);
        body = NULL;
    }

    // close file
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    // free request
    if (request != NULL)
    {
        free(request);
        request = NULL;
    }

    // close client's socket
    if (cfd != -1)
    {
        close(cfd);
        cfd = -1;
    }
}

/**
 * Starts server.
 */
void start(short port, const char* path)
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)
    {
        stop();
    }

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        stop();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        stop();
    }

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        stop();
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        stop();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        stop();
    }
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // reset server's state
    reset();

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // terminate process
    if (errsv == 0)
    {
        // success
        exit(0);
    }
    else
    {
        // announce error
        printf("\033[33m");
        printf("%s", strerror(errsv));
        printf("\033[39m\n");

        // failure
        exit(1);
    }
}


