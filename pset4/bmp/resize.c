/**
 * resize.c
 * 
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP.
 */

#include <stdio.h>
#include <cs50.h>

#include "bmp.h"

int main (int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Incorrect Usage!\nUsage: ./resize n infile outfile\n");
        return 1;
    }
        
    // get resize factor
    int n = atoi(argv[1]);
    
    // exit if n is 0 or more than 100
    if (n > 100 || n < 1)
    {
        printf("Resize factor must be within [1,100].\n");
        return 1;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // determine padding for scanlines for original BMP
    int old_padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // height and width of original BMP
    int old_width = bi.biWidth;
    int old_height = abs(bi.biHeight);
    
    // modify BITMAPINFOHEADER
    bi.biWidth = n * bi.biWidth;
    bi.biHeight = n * bi.biHeight;
    
    // determine padding for scanlines of resized BMP
    int new_padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // calculate image size for bi.biSizeImage
    if (bi.biHeight < 0)
    {
            bi.biSizeImage = (bi.biWidth * bi.biHeight * (-1)) * (bi.biBitCount / 8) + (new_padding * abs(bi.biHeight));   
    }
    
    else
                bi.biSizeImage = (bi.biWidth * bi.biHeight ) * (bi.biBitCount / 8) + (new_padding * abs(bi.biHeight));   

    // modify BITMAPFILEHEADER
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
        
    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0 ; i < old_height; ++i)
    {
        int scale = n;
        
        // scale horizontally
        while (scale > 0)
        {
            scale = scale - 1; 
            
            // iterate over pixels in scanline
            for (int j = 0; j < old_width; j = j + 1)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                int scale = n;
                
                // resize horizontally
                while (scale > 0)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    scale = scale - 1;
                }
                
            }
            
            // add padding
            for (int k = 0; k < new_padding; k++)
            {
                fputc(0x00, outptr);
            }
            
            int offset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) 
                         + i * (old_width * sizeof(RGBTRIPLE)) + i * old_padding;
            
            // go back for horizontal scaling
            fseek(inptr, offset, SEEK_SET);
        }
        
        // skip over padding, if any
        fseek(inptr, (old_width * sizeof(RGBTRIPLE)) + old_padding, SEEK_CUR);   
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
} 
