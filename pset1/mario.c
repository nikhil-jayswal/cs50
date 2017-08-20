/**
*
*mario.c
*
*Nikhil Jayswal
*
*Builds a pyramid of required height
*
*/


#include <stdio.h>
#include<cs50.h>

int main (void)
{
    int pyrm_height;
    
    do
    {
        printf("Please enter the required height of the pyramid (max 23): ");
        pyrm_height = GetInt();
    }
    while(pyrm_height <0 || pyrm_height > 23);
    
    int spaces = pyrm_height-2;
    
    // build the pyramid
    
    for(int i=1;i<=pyrm_height;++i)
    {
        for(int j=0;j<=pyrm_height;++j)
        {
            if(j<=spaces)
               printf(" ");
            else
               printf("#");
        }
        printf("\n");
        spaces--;
    }
}

