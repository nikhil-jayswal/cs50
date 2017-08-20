/******
 *
 *   testst5.c 
 *   
 *   copyright 2015 ebobtron(aka Robert Clark)
 *  
 *   test performance for pset5 speller.c
 *
 ******/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>


int main(int argc, char* argv[])
{
    // test command line arguments, bitch, quit if bad
    if (argc < 2 || argc > 3)
    {
       printf("Usage: ./test ./speller id submission_name\n");
       return 1;
    }
    
    // define and initialize some floating point variables
    double totalloadtime = 0;
    double totalchecktime = 0;
    double totalsizetime = 0;
    double totalunloadtime = 0;
    double totaltime = 0;
    
    // define a couple of arrays
    double vals[8];
    char linebuf[30];    
    
    // define a path variable  pset6 for 2014 and pset5 for 2015
    char* path = "/home/cs50/pset5/texts/";
    short int pathSize = strlen(path);
    
    // get two glob data objects
    glob_t data;
    glob_t data2;
    
    // collect the names of all text files for  
    switch(glob("/home/cs50/pset5/texts/*.txt", 0, NULL, &data ))
    {
        case 0:
            break;
        case GLOB_NOSPACE:
            printf( "Out of memory\n" );
            break;
        case GLOB_ABORTED:
            printf( "Reading error\n" );
            break;
        case GLOB_NOMATCH:
            printf( "No files found\n" );
            break;
        default:
            break;
    }
    
    // repeat glob use to check if results exists, make it if not.
    switch(glob("results", 0, NULL, &data2 ))
    {
        case 0:
            break;
        case GLOB_NOSPACE:
            printf( "Out of memory\n" );
            break;
        case GLOB_ABORTED:
            printf( "Reading error\n" );
            break;
        case GLOB_NOMATCH:
            printf( "directory /results not found\n" );
            printf( "making /results \n" );
            system("mkdir results");
            break;
        default:
            break;
    }
    
    // define some character buffers, some people call strings
    char com[200];
    char fname[100];
    
    //                     data.gl_pathc
    // iterate over the glob text file count
    for(int i = 0; i < data.gl_pathc; i++)
    {
        short int fcount = 0;
        for(int j = pathSize; j < strlen(data.gl_pathv[i]); j++)
        {
           fname[fcount] = data.gl_pathv[i][j];
           fname[fcount+1] ='\0';
           fcount++;
        }  
        
        // create the test command line string passing the output of 
        // speller to a file named for the text file being tested
        sprintf(com, "%s %s > results/%s", argv[1], data.gl_pathv[i], fname);
        
        // example
        // ./speller pset path > results/filename.txt
        
        // execute the above command line
        system(com);
    }
    
    // prepare a data buffer, u may call it a file
    FILE* outfile = fopen("data.txt","a+");

    // collect the data        -> data.gl_pathc
    int ntest = 0;
    for(int i = 0; i < data.gl_pathc; i++)
    {
        short int fcount = 0;
        for(int j = pathSize; j < strlen(data.gl_pathv[i]); j++)
        {
           fname[fcount] = data.gl_pathv[i][j];
           fname[fcount+1] ='\0';
           fcount++;
        }
        sprintf(com, "results/%s", fname);
        FILE* infile = fopen(com, "r");
        if (infile)
        {
            fseek(infile,-150, SEEK_END);
            
            fgets(linebuf, sizeof(linebuf),infile);
            for(int j =  0; j < 6; j++)
            {
                fgets(linebuf, sizeof(linebuf),infile);
                int k = 20;
                int k2 = 0;
                while(linebuf[k] != '\n')
                {
                    com[k2] = linebuf[k];
                    k++;
                    k2++;
                }
                com[k2] = '\0';
                vals[j] = atof(com);
            } 
        }

    
        if (infile)
        {
            fclose(infile);
        }
 
        totalloadtime = totalloadtime + vals[0];
        totalchecktime = totalchecktime + vals[1];
        totalsizetime = totalsizetime + vals[2];
        totalunloadtime = totalunloadtime + vals[3];
        totaltime = totaltime + vals[4];
        
        //
        if (argc == 3)
        { 
            sprintf(com, "%s, %1.4f, %1.4f, %1.4f, %1.4f, %1.4f\n", fname, vals[4], vals[0], vals[1], vals[2], vals[3]);
        printf("%s", com);
        }
        ntest++;
        
    } 
    sprintf(com, "%1.4f, %1.4f, %1.4f, %1.4f, %1.4f\r\n", totaltime/ntest, totalloadtime/ntest, totalchecktime/ntest, totalsizetime/ntest, totalunloadtime/ntest);
    
    fwrite(com, strlen(com), 1, outfile);
    fclose(outfile);
    
    printf("%s", com);
    
    globfree(&data);
    return 0;
}
