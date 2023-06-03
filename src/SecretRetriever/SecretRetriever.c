//
// Created by shadad on 03/06/23.
//
#include "SecretRetriever.h"
static shadow * fromImageToShadow(uint8_t k ,bmpFile * imageFile);
static uint8_t  * interpolate(uint8_t * aPoints, uint8_t * bPoints);
static void checkCoefficients(uint8_t  k ,uint8_t * coefficients);



shadowGenerator * initRetriever(struct params * params){
    struct shadowGenerator * shadowGenerator = malloc(sizeof(shadowGenerator));
    shadowGenerator->k = params->k;
    shadowGenerator->n = params -> n;
    openDirectory(shadowGenerator, params->directory);
    shadowGenerator->creatingFileName = malloc(strlen(params->file));
    strcpy(shadowGenerator->creatingFileName, params->file);
    return shadowGenerator;
}



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

void retrieveSecret(shadowGenerator * generator){
    uint8_t  k = generator->k ;
    uint8_t  * imagePointer = generator->file->pixels;
    int currentBlock = 0;

    uint8_t  * aPoints = malloc(generator->k);
    uint8_t  * bPoints = malloc(generator->k);

    while( currentBlock < generator->generatedShadows[0]->shadowNumber){

        for (int i = 0; i < k ; i ++){
            int shadowNumber = generator->generatedShadows[i]->shadowNumber;
            aPoints[shadowNumber] = generator->generatedShadows[i]->points[currentBlock];
            bPoints[shadowNumber] = generator->generatedShadows[i]->points[currentBlock + 1];
        }
        uint8_t * coefficients = interpolate(aPoints, bPoints);
        checkCoefficients(generator->k, coefficients);
        memcpy(imagePointer, coefficients, k);
        memcpy(imagePointer, coefficients + k + 2, k - 2);

        imagePointer += (2*k) - 2;
        currentBlock ++ ;
    }


    int fd = open(generator->creatingFileName, O_WRONLY | O_CREAT);
    if (fd == -1) {
        perror("open");
        return ;
    }
    write(fd, generator->file, generator->file->header->imageSize);

    close(fd);
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


static uint8_t  * interpolate(uint8_t * aPoints, uint8_t * bPoints){
    return NULL;
}


void checkCoefficients(uint8_t  k ,uint8_t * coefficients){
    int valid = 0;
    for (int i = 0; i < 255; i++){
        if ( (coefficients[k] = Z_p(- i * coefficients[0])) &&
                (coefficients[k+1] = Z_p(- i * coefficients[1]))
        )
            valid = 1;
    }
    if (! valid){
        printf("One invalid shadow was provided. ")
        exit(-1);
    }

    return ;
}