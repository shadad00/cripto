//
// Created by shadad on 31/05/23.
//

#ifndef CRIPTO_BMP_H
#define CRIPTO_BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

typedef struct bmpHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelsOffset;
    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compressionMethod;
    uint32_t imageSize;
    uint32_t xPixelsPerMeter;
    uint32_t yPixelsPerMeter;
    uint32_t numColors;
    uint32_t importantColors;
} bmpHeader;

#pragma pack()


typedef struct bmpFile{
    bmpHeader  * header;
    uint8_t * pixels;
    int fd ;
} bmpFile;



//used to parse a bmp file.
bmpFile *  openBmpFile(const char * path);


#endif //CRIPTO_BMP_H
