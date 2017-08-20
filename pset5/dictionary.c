/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <cs50.h>

#include "dictionary.h"

// global variable - start of the dictionary
node* first = NULL;

// global variable - number of words in dictionary
unsigned int num = 0;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    node* temp = first; // navigation pointer
    
    for (int i = 0; word[i] != '\0'; ++i)
    {
        int pos = 0;
        // calculate position
        if (word[i] != '\'')
            pos = (word[i] % 32) - 1;
        else
            pos = 26;
        
        // if end of word but not in uppercase in dictionary
        if (word[i+1] == '\0')
        {
            if (!(((temp + pos) -> ch) >= 'A' && ((temp + pos) -> ch) <= 'Z'))
                return false;
        }
        else
        {
            // go to alphabet
            temp = temp + pos;
            
            // if pointer leads nowhere
            if (temp -> ptr == NULL)
                return false;
            
            // else follow pointer
            temp = temp -> ptr;
        }
    }
    
    return true;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // open dictionary
    FILE* file = fopen(dictionary, "r");
    
    // check if dictionary was opened properly
    if (file == NULL)
    {
        printf("Could not load %s.\n", dictionary);
        return false;
    }
    
    // start loading dictionary
    // remember dictionary is in lowercase only
    char word[LENGTH + 1];
    int index = 0;
    
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        // if c is an alphabet or an apostrophe
        if (c != '\n')
        {
            word[index] = c;
            index++;
        }
        
        // if c is anything else, i.e. a \n indicating end of a word
        else
        {
            // update number of words in dictionary
            num++;
            
            // pointers for navigation
            node* current = first; // supposed to be at the current position
                                   // of insertion
            node* next = first; // supposed to hold root address of next level
                
            // insert word in trie
            for (int i = 0; i < index; ++i)
            {
                char ch = word[i];
                
                // check if trie exists
                if (first == NULL)
                {
                    first = calloc(27, sizeof(node));
                    current = first;
                    next = first;
                }
                
                int pos = 0 ;
                
                // calculate insertion position 
                if (ch != '\'')
                    pos = ch - 'a';
                else
                    pos = 26;
                
                // current still points to previous alphabet
                // update current and in the process link the two alphabets
                if (i == 0) // if current = first
                    current = current + pos;
                else
                {
                    current -> ptr = next;
                    current = next + pos;
                }
                               
                // insert the character into proper position
                                               
                // if end of word
                if (i == (index - 1))
                {
                    // change to uppercase
                    current -> ch = 'A' + (ch % 32) - 1; 
                    break;
                }
                
                // if not end of word
                // if the node is empty
                if (current -> ch == '\0')
                    current -> ch = ch;   
                                
                // check whether next level of trie exists
                if (current -> ptr != NULL)
                    next = current -> ptr;
               else // build the next level
                    next = calloc(27, sizeof(node));
            }
            
            // reset index
            index = 0;    
        }
            
    }
    
    // check for errors 
    if (ferror(file))  
    {
        fclose(file);
        printf("Error reading %s.\n", dictionary);
        return false;
    } 
    
    fclose(file);
    
    return true; 
}    

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    if (first ==  NULL)
    {
        printf("Dictionary not loaded!");
        return 0;
    } 
        
    return num;
}

/**
 * Does the work supposed to be done by unload()
 */
bool mem_free(node* first)
{
    if (first ==  NULL)
    {
        printf("Dictionary not loaded!");
        return true;
    } 
    
    node* temp = NULL;
    bool a = false;
    // recursively free levels
    // free a level if no node has any valid pointers
    for (int i = 0; i < 27; ++i)
    {
        if((first + i) -> ptr != NULL)
        {
            temp = (first + i) -> ptr;
            a = mem_free(temp);
            
            // if something goes wrong
            if (a == false)
                return false;
        }  
    }
    
    free(first);
    return true;   
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    if (mem_free(first) == true)
        return true;
    
    else
        return false;
}
