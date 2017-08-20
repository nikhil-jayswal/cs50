/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */
       
#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    // TODO: implement a searching algorithm
     
    if (n < 1)
        return false;
           
    int start = 0;
    int end = n-1;
        
    while (start < end)
    {
    	int mid = (start + end)/2;
    	
    	if (values[mid] == value)
    		return true;
    	
    	else if (values[mid] > value)
    		end = mid - 1;
    	
    	else
    		start = mid + 1;
    }
    
    if (end == start)
    {
    	if (values[end] == value)
    		return true;
    }
    
    return false;
    
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // TODO: implement an O(n^2) sorting algorithm
  
    int swaps = 0;
    
    do
    {
    	swaps = 0;
    	
    	for (int i = 0; i < n-1; ++i)
    	{
    		if (values[i] > values[i+1])
    		{	
    			// swap
    			int tmp = values[i];
    			values[i] = values[i+1];
    			values[i+1] = tmp; 
    			
    			swaps++;
    		}
    	}
    	    	
    } while (swaps > 0);
        
    return;
}
