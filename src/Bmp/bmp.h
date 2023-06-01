//
// Created by shadad on 31/05/23.
//

#ifndef CRIPTO_BMP_H
#define CRIPTO_BMP_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>




typedef struct tagBITMAPFILEHEADER {
    uint16_t bfType; //The file type; must be BM.
    uint32_t bfSize; // The size, in bytes, of the bitmap file.
    uint16_t bfReserved1; //must be zero, then will be used to store shadow number
    uint16_t bfReserved2; // must be zero
    uint32_t bfOffBits;  // byte offset from the beginning of the header to the bitmap bits.
} BITMAPFILEHEADER;

typedef struct BITMAPFILE{
    BITMAPFILEHEADER  * header;
    uint8_t * pixels;
} BITMAPFILE;


BITMAPFILE *  openBmpFile(const char * path);


#endif //CRIPTO_BMP_H
