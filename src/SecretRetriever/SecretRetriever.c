//
// Created by shadad on 03/06/23.
//
#include "SecretRetriever.h"
static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile);


void initializeShadows(shadowGenerator * generator){
    shadow ** parsedShadows = malloc(generator->k * (sizeof(shadow *) ));

    bmpFile  * currentImageFile;
    for (int i = 0 ; i < generator -> k ; i ++){
        currentImageFile = openBmpFile(generator->imageFiles[i]);
        parsedShadows[i] = fromImageToShadow(generator->k, currentImageFile);
    }

    //copy the header file for the generating image.
    generator->file = calloc(currentImageFile->header->fileSize, 1);
    memcpy(generator->file, currentImageFile->header, currentImageFile->header->fileSize - currentImageFile->header->imageSize);

    generator->generatedShadows = parsedShadows;
}



static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile){
    shadow * shadow = malloc(sizeof (shadow));
    shadow->shadowNumber = imageFile->header->reserved1;
    shadow->pointNumber = imageFile->header->imageSize / (k -1);
    shadow->points = malloc(shadow->pointNumber);

    int lsb4 = ( k == 3 || k == 4 ) ? 1 : 0;
    int shadowGenByImage= ( lsb4 ) ? 2 : 4;
    int shifter = lsb4 ? 4 : 2 ;

    int current = 0;
    while(current < shadow->shadowNumber){
        shadow->points[current] = 0;
        for(int i = 0 ; i < shadowGenByImage; i++){
            shadow->points[current] += imageFile->pixels[i] >>  (shifter * (shadowGenByImage - i) );
            shadow->points[current] = shadow->points[current] << shifter;
        }
        current++;
    }

    return shadow;
}
