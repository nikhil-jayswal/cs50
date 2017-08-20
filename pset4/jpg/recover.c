/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <cs50.h>
#include <string.h>

bool check(char* block);

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc > 1)
    {
        printf("Usage: ./recover\n");
    }
    
    // open CF's forensic image
    FILE* file = fopen("card.raw", "r");
    
    // check if file was opened properly
    if (file == NULL)
    {
        printf("Could not open file!\n");
        return 1;
    }
             
    // keep track of number of recovered images
    int found = 0;
    
    // keep track of file's end
    int near_end = 0;
    
    // create a memory space to hold a FAT block
    char* block = malloc(512);
                   
    if (block == NULL)
    {
        printf("Pointer Error\n");
        return 1;
    }
    
    while (fread(block, 512, 1, file) == 1)
    {
        // check for start of a jpeg
        if (check(block) == true)
        {
            // string to hold filename
            char filename[8];
            for (int i = 0; i < 8; ++i)
                filename[i] = '\0';
                
            // open a new jpeg file
            if (found < 10) 
                sprintf(filename, "00%d.jpg", found);
            
            else
                sprintf(filename, "0%d.jpg", found);
                        
            // open jpeg file in append mode
            FILE* img = fopen(filename, "a");
            
            // update nummber of found images
            found++;
            
            // copy jpeg into the jpeg file
            do
            {
                fwrite(block, 512, 1, img);
                
                // read again only if not at end of file
                if (fread(block, 512, 1, file) == 1);
                                
                else
                    {
                        near_end = 1;
                        break;
                    };
            } 
            while(check(block) == false);
            
            // close image
            fclose(img);
            
            // if at end of file, exit the loop
            if (near_end != 1)
                fseek(file, 512 * (-1), SEEK_CUR);
            
            else
                break;
                
        }
          
    }
       
    // free memory
    free(block);
    
    // close file
    fclose(file);
    
    return 0;
}

/**
 * Checks for the start of a jpeg
 */
bool check(char* block)
{
    char jpeg[5];
    char jpeg1[5];
    {
        jpeg[0] = 0xff;
        jpeg[1] = 0xd8;
        jpeg[2] = 0xff;
        jpeg[3] = 0xe0;
        jpeg[4] = '\0';
            
        jpeg1[0] = jpeg[0];
        jpeg1[1] = jpeg[1];
        jpeg1[2] = jpeg[2];
        jpeg1[3] = 0xe1;
        jpeg1[4] = '\0';
    }
    
    char str[5];
    str[4] = '\0';
    
    strncpy(str, block, 4);
    
    if (strcmp(str, jpeg) == 0 || strcmp(str, jpeg1) == 0)
        return true;
    
    return false;    
}

