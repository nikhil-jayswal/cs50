/**
*initials.c
*
*Nikhil Jayswal
*nikhil.nj28@gmail.com
*
*Prints initials of the user in uppercase
*/

#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main (void)
{
    // get user's name
    string name;
    //printf("Enter your name: ");
    name = GetString();
    
    // get length of name
    int length = 0;
    length = strlen(name);
    
    // print initials in uppercase
    for (int i = 0; i < length; ++i)
    {
        // first letter in the name
        if (i == 0)
            printf("%c", toupper(name[i]));
        
        else if (name[i-1] == ' ')
            printf("%c", toupper(name[i]));
    }
    
    printf("\n");
    
    return 0;
}
