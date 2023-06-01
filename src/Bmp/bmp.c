//
// Created by shadad on 31/05/23.
//

#include "bmp.h"

BITMAPFILE *  openBmpFile(const char * path){
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    struct stat fileStats;
    if (fstat(fd, &fileStats ) != 0){
        perror("fstat");
        return NULL;
    }

    int fileSize = fileStats.st_size;

    void *   filePointer = malloc(fileSize );
    if ( read(fd, filePointer  ,fileSize) != fileSize){
        perror("Unable to read file");
        return NULL;
    }

    BITMAPFILEHEADER * headerPointer = (BITMAPFILEHEADER *) filePointer;
    int bitMapOffset = headerPointer->bfOffBits;
    uint8_t * bitMapPointer = ((uint8_t * )filePointer) + bitMapOffset;

    BITMAPFILE *  bitMapFile = malloc(sizeof (BITMAPFILE));
    bitMapFile->header = headerPointer;
    bitMapFile->pixels = bitMapPointer;
    return bitMapFile;
}

