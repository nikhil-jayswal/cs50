/****************************************************************************
 * dictionary.h
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Declares a dictionary's functionality.
 ***************************************************************************/

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// definition of a "node"
typedef struct node
{
    char ch;
    struct node* ptr;
}
node;

// maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word);

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary);

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void);

/**
 * Does the work supposed to be done by unload()
 */
bool mem_free(node* first);

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void);

#endif // DICTIONARY_H
