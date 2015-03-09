#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libpnm.h"


void Decode_Using_LZ77(char *in_compressed_filename_Ptr, char *out_PGM_filename_Ptr)
{

}

void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, char*compressed_filename_Ptr)
{
    struct PGM_Image pgmImage;
    load_PGM_Image(&pgmImage, in_PGM_filename_Ptr);
    int index = 0;
    int iIndex;
    int endPtrAdd;
    int indexBefore = 1;
    int width;
    int height;
    int encodeCont = 1;
    int cont = 1;
    int startPtr = 0;
    int endPtr = 0;
    int *searchBuffer = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int indexArr = 0;

    int *offSetArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *repArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *misAllArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));

    index = 0;
    for(height = 0; height < pgmImage.height; height++)
    {
        for(width = 0; width < pgmImage.width; width++)
        {
            searchBuffer[index] = pgmImage.image[height][width];

            index++;
        }
    }

    int i;

    index = 0;

    while(encodeCont == 1 && endPtr < (pgmImage.width * pgmImage.height - 1))
    {
        iIndex = endPtr;
        for(i = endPtr-1; i >= startPtr; i--)
        {
            endPtrAdd = 1;

            if(searchBuffer[i] == searchBuffer[endPtr])
            {
                // printf("%d %d\n", searchBuffer[i], searchBuffer[endPtr]);

                indexBefore = 1;
                cont =1;
                while(cont == 1)
                {
                    if((searchBuffer[i + indexBefore] == searchBuffer[endPtr + indexBefore]) && i+indexBefore < endPtr)
                    {
                        indexBefore++;

                    }
                    else
                    {
                        if(indexBefore > index)
                        {
                            index = indexBefore;
                            iIndex = i;
                            //printf("%d %d\n", endPtr, iIndex);
                            endPtrAdd = 2;
                        }

                        cont = 0;
                    }
                }

            }

        }

        misAllArr[indexArr] = searchBuffer[endPtr + index];
        repArr[indexArr] = index;
        offSetArr[indexArr] = endPtr - iIndex;
        indexArr++;
        endPtr = endPtr + index + 1;
        index = 0;
        //special case at start of encoding
        if(endPtr == 0 && startPtr == 0)
        {
            misAllArr[indexArr] = searchBuffer[endPtr];
            offSetArr[indexArr] = 0;
            repArr[indexArr] = 0;
            indexArr++;
            endPtr++;
        }

        if(endPtr > searching_buffer_size)
        {
            startPtr = endPtr - searching_buffer_size;
        }

        //printf("%d\n", endPtr - startPtr);
    }


    int j;



    //place arrays and height and width into encoded file

    //free arrays
    free(searchBuffer);
    free(offSetArr);
    free(repArr);
    free(misAllArr);

}

int main(int argc, char *argv[])
{
    /*Inputs:
    * Buffer Size - Integer value
    * Function - String - Encode or Decode
    * PGM Name - String - PGM image to encode
    * Compressed file - String - File to output
    */
    int bufferSize;
    char *function;
    char *pgmFilename;
    char *compressedFileName;

    if(argc!=5)
    {
        perror("Invalid command line inputs");
        exit(1);
    }

    bufferSize = atoi(argv[1]);
    function = strdup(argv[2]);
    pgmFilename = strdup(argv[3]);
    compressedFileName = strdup(argv[4]);

    if(strcmp(function, "Encode") == 0)
    {
        Encode_Using_LZ77(pgmFilename, bufferSize, compressedFileName);
    }
    else
    {
        Decode_Using_LZ77(compressedFileName, strcat(compressedFileName, ".pgm"));
    }


}
