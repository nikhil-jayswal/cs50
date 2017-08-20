/**
*vigenere.c
*
*Nikhil Jayswal
*nikhil.nj28@gmail.com
*
*Prints cipher text using Vigenere's cipher
*/

#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main (int argc, string argv[])
{
	// prompt user if incorrect number of command-line arguments are passed
	if(argc!=2)
	{
		printf("Enter a key and ONE KEY ONLY! Try again!\n");
		return 1;
	}
	
	string key = argv[1];
	
	// check key for non-aphabetic characters
	for(int j=0,n=strlen(key);j<n;++j)
	{
		if(!isalpha(key[j]))
		{
			printf("Enter a ALPHABETICAL key! Try again!\n");
			return 1;
		}
	}
	
	string plain_text;
	
	// get plain text from user
	// printf("Enter the plaintext: ");
	plain_text = GetString();
	
	// generate cipher text
	int m=strlen(key);
	for (int i=0,k=0, n=strlen(plain_text);i<n;++i)
	{
		int j=0;
		
		if(k>=m)                              // re-cycle key
			k=0;
		
		// change key's alphabets to numbers
		if (isupper(key[k]))
			j=key[k]-'A';
		else if (islower(key[k]))
			j=key[k]-'a';
		
		
		if (isupper(plain_text[i])) 
		{
			while(j>0)
			{
				if(plain_text[i]=='Z')       // wrap-around from Z to A
					plain_text[i]=plain_text[i]-26;
				
				++plain_text[i];
				--j;
			}
			++k;
		}
		else if (islower(plain_text[i]))
		{
			while(j>0)
			{
				if(plain_text[i]=='z')       // wrap-around from Z to A
					plain_text[i]=plain_text[i]-26;
				
				++plain_text[i];
				--j;
			}
			++k;
		}
	}	
	
	printf("%s\n",plain_text);
	
	return 0;	
}
