#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libpnm.h"


void Decode_Using_LZ77(char *in_compressed_filename_Ptr, char *out_PGM_filename_Ptr)
{
    int height;
    int width;
    int arrLength;
    FILE *decodeFile;
    char line[80];
    int endPtr;
    int startPtr;

    decodeFile = fopen(in_compressed_filename_Ptr, "rt");
    //get height
    fgets(line, 80, decodeFile);
    sscanf(line, "%d", &height);

    //get width
    fgets(line, 80, decodeFile);
    sscanf(line, "%d", &width);

    //get array length
    fgets(line, 80, decodeFile);
    sscanf(line, "%d", &arrLength);

    struct PGM_Image pgmImage;
    create_PGM_Image(&pgmImage, width, height, 255);

    int *offSetArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *repArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *misAllArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *searchBuffer = malloc((pgmImage.width * pgmImage.height) * sizeof(int));

    int i;
    int temp;
    //get offSetArr
    for(i=0; i< arrLength;i++){
        fgets(line, 80, decodeFile);
        sscanf(line, "%d", &temp);
        offSetArr[i] = temp;
    }

    //get repArr
    for(i=0; i< arrLength;i++){
        fgets(line, 80, decodeFile);
        sscanf(line, "%d", &temp);
        repArr[i] = temp;
    }

    //get misArr
    for(i=0; i< arrLength;i++){
        fgets(line, 80, decodeFile);
        sscanf(line, "%d", &temp);
        misAllArr[i] = temp;
    }

    int j;
    int tempPtr;
    height = 0;
    width =0;
    endPtr = 0;
    startPtr = 0;
    for(i=0;i<arrLength;i++){
        //no repeating pattern. Value equal to misAllArr[i]
        if(offSetArr[i] == 0 && repArr[i] == 0){
            searchBuffer[endPtr] = misAllArr[i];
            endPtr++;
        }else{//there is a repeating pattern
            tempPtr = endPtr;
            for(j = tempPtr - offSetArr[i]; j<tempPtr - offSetArr[i] + repArr[i];j++){
                searchBuffer[endPtr] = searchBuffer[j];
                endPtr++;
            }
            searchBuffer[endPtr] = misAllArr[i];
            endPtr++;
        }

    }


    int index = 0;
    for(height = 0; height < pgmImage.height; height++)
    {
        for(width = 0; width < pgmImage.width; width++)
        {
            pgmImage.image[height][width] = searchBuffer[index];

            index++;
        }
    }

    save_PGM_Image(&pgmImage, out_PGM_filename_Ptr, 1);
    free_PGM_Image(&pgmImage);
    //free arrays
    free(searchBuffer);
    free(offSetArr);
    free(repArr);
    free(misAllArr);
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

    int *offSetArrFreq = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *repArrFreq = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
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



    //place arrays and height and width into encoded file

    FILE *encodedFile;
    encodedFile = fopen(compressed_filename_Ptr, "w");
    if(encodedFile == NULL)
    {
        printf("Error writing to file\n");
        exit(1);
    }

    //print height and width of image to file
    fprintf(encodedFile, "%d\n", pgmImage.height);
    fprintf(encodedFile, "%d\n", pgmImage.width);
    //print array length of three arrays
    fprintf(encodedFile, "%d\n", indexArr);

    //print arrays in order of offset, rep, mis
    int j;
    for(j=0; j<indexArr; j++)
    {
        fprintf(encodedFile, "%d\n", offSetArr[j]);
        offSetArrFreq[offSetArr[j]] = offSetArrFreq[offSetArr[j]]+1;
    }
    for(j=0; j<indexArr; j++)
    {
        fprintf(encodedFile, "%d\n", repArr[j]);
        repArrFreq[repArr[j]] = repArrFreq[repArr[j]]+ 1;
    }
    for(j=0; j<indexArr; j++)
    {
        fprintf(encodedFile, "%d\n", misAllArr[j]);
    }
    fclose(encodedFile);

//
//    char *offSetFileName;
//    offSetFileName = strdup(compressed_filename_Ptr);
//    strcat(offSetFileName, ".offSetFreq");
//    FILE *offSetFrequency;
//    offSetFrequency = fopen(offSetFileName, "w");
//    for(j=0;j<searching_buffer_size; j++){
//        fprintf(offSetFrequency, "%d\n", j);
//    }
//    for(j=0;j<searching_buffer_size; j++){
//        fprintf(offSetFrequency, "%d\n", offSetArrFreq[j]);
//    }
//    fclose(offSetFrequency);
//
//    char *repFileName;
//    repFileName = strdup(compressed_filename_Ptr);
//    strcat(repFileName, ".repFreq");
//    FILE *repFrequency;
//    repFrequency = fopen(repFileName, "w");
//    for(j=0;j<searching_buffer_size; j++){
//        fprintf(repFrequency, "%d\n", j);
//    }
//    for(j=0;j<searching_buffer_size; j++){
//        fprintf(repFrequency, "%d\n", repArrFreq[j]);
//    }
//    fclose(repFrequency);


    //free arrays
    free(searchBuffer);
    free(offSetArr);
    free(repArr);
    free(misAllArr);
    free(offSetArrFreq);
    free(repArrFreq);

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


    function = strdup(argv[1]);
    if(strcmp(function, "encode") == 0)
    {
        if(argc!=5)
        {
            perror("Invalid command line inputs");
            exit(1);
        }

        bufferSize = atoi(argv[2]);
        pgmFilename = strdup(argv[3]);
        compressedFileName = strdup(argv[4]);
        Encode_Using_LZ77(pgmFilename, bufferSize, compressedFileName);
    }
    else if(strcmp(function, "decode") == 0)
    {
        if(argc!=4)
        {
            perror("Invalid command line inputs");
            exit(1);
        }
        compressedFileName = strdup(argv[2]);
        pgmFilename = strdup(argv[3]);
        Decode_Using_LZ77(compressedFileName, pgmFilename);
    }




}
