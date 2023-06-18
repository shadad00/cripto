//
// Created by shadad on 31/05/23.
//

#include "./bmp.h"

bmpFile *  openBmpFile(const char * path){
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat fileStats;
    if (fstat(fd, &fileStats ) != 0){
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int fileSize = fileStats.st_size;


    void * filePointer = malloc(fileSize );

    if ( read(fd, filePointer  ,fileSize) != fileSize){
        perror("Unable to read file");
        exit(EXIT_FAILURE);
    }

    bmpHeader * headerPointer = (bmpHeader *) filePointer;
    uint32_t pixelsOffset = headerPointer->pixelsOffset;

    uint8_t * pixelsPointer = ((uint8_t * )filePointer) + pixelsOffset;

    bmpFile *  bitMapFile = malloc(sizeof (bmpFile));
    bitMapFile->header = headerPointer;
    bitMapFile->pixels = pixelsPointer;
    bitMapFile -> fd = fd;
    return bitMapFile;
}

