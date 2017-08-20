/**
 * greedy.c
 *
 * Nikhil Jayswal
 * nikhil.nj28@gmail.com
 *
 * Created : 30/01/2013  
 * 
 * Greedy algorithm for calculating minimum number of coins for change 
 *
 * Uses roundf() to avoid floating point errors
 *
 */

#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main (void)
{
	float amount = 0.0;
	int int_amount = 0;
    
	// get a non-negative amount from user
	do
	{
    		printf("How much change is owed?: ");
     		amount = GetFloat();
	}
	while(amount<0);
	
    // convert amount to cents and round off to avoid floating point errors
    amount = 100 * amount;
    int_amount = (int)roundf(amount);
    
    int coins = 0;
    
    while (int_amount >= 25)
    {
        int_amount = int_amount - 25;
        coins++;
    }
    
    while (int_amount >= 10)
    {
        int_amount = int_amount - 10;
        coins++;
    }
    
    while (int_amount >= 5)
    {
        int_amount = int_amount - 5;
        coins++;
    }
    
    while (int_amount >= 1)
    {
        int_amount = int_amount - 1;
        coins++;
    }
    
    printf("%d\n", coins);
 
}

     	

	
      	
      	
     
     
    
    

