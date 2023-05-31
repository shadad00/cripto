//
// Created by shadad on 31/05/23.
//

#include "bmp.h"

BITMAPFILE openBmpFile(const char * path){
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }
    FILE *file = fdopen(fd, "r");
    if (file == NULL) {
        perror("Failed to convert file descriptor");
        return NULL;
    }

    BITMAPFILEHEADER *   header = malloc(sizeof(BITMAPFILEHEADER) );
    if ( fgets(header,sizeof(BITMAPFILEHEADER) + 1 ,file) != NULL){
        perror("Unable to retrieve header information.")
        return NULL;
    }

    uint32_t  offset = header->bfOffBits;
    uint8_t  * bitmap = malloc(header->bfSize);


}

