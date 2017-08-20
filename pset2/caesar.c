/**
*caesar.c
*
*Nikhil Jayswal
*nikhil.nj28@gmail.com
*
*Prints cipher text using Caesar's cipher
*/

#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main (int argc, string argv[])
{
	// prompt user if incorrect number of command-line arguments are passed
	if(argc!=2 )
	{
		printf("Enter a key and ONE KEY ONLY! Try again!\n");
		return 1;
	}
		
	
	int key = atoi(argv[1]);
	string plain_text;
	
	// get plain text from user
	// printf("Enter the plaintext: ");
	plain_text = GetString();
	
	// generate cipher text
	for (int i=0, n=strlen(plain_text);i<n;++i)
	{
		if (isalpha(plain_text[i])) 
			{
				int j=key;
				while(j>0)
				{
					if(plain_text[i]=='Z' || plain_text[i]=='z')            // wrap-around from Z to A
						plain_text[i]=plain_text[i]-26;
					
					plain_text[i]++;
													
					--j;
				}
			}
	}	
	
	printf("%s\n",plain_text);
	
	return 0;	
}
