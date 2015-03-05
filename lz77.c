#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libpnm.h"


void Decode_Using_LZ77(char *in_compressed_filename_Ptr, char *out_PGM_filename_Ptr) {

}

void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, char*compressed_filename_Ptr) {
    struct PGM_Image pgmImage;
    load_PGM_Image(&pgmImage, in_PGM_filename_Ptr);
    int index;
    int width;
    int height;
    int startPtr = 0;
    int endPtr = 0;
    int *searchBuffer = malloc((pgmImage.width * pgmImage.height) * sizeof(int));

    int *offSetArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *repArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));
    int *misAllArr = malloc((pgmImage.width * pgmImage.height) * sizeof(int));

    index = 0;
    for(height = 0; height < pgmImage.height; height++){
        for(width = 0; width < pgmImage.width; width++){
            searchBuffer[index] = pgmImage.image[width][height];
            index++;
        }
    }

    while(endPtr != index){

    }

}

int main(int argc, char *argv[]){
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

    if(strcmp(compressedFileName, "Encode") == 0) {
        Encode_Using_LZ77(pgmFilename, bufferSize, compressedFileName);
    }else{
        Decode_Using_LZ77(compressedFileName, strcat(compressedFileName, ".pgm"));
    }


}
