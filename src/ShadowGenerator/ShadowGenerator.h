//
// Created by shadad on 03/06/23.
//

#ifndef CRIPTO_SHADOWGENERATOR_H
#define CRIPTO_SHADOWGENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include "../Bmp/bmp.h"
#include "../utils.h"
#include "../ModularOperations.h"



typedef struct shadow{
    uint8_t shadowNumber;
    uint64_t pointNumber;
    uint8_t * points;
} shadow;

typedef struct shadowGenerator{
    bmpFile * file;
    uint8_t  k;
    uint8_t  n;
    shadow ** generatedShadows;
    char ** imageFiles;
    char * creatingFileName;
} shadowGenerator;

shadowGenerator * initialize(struct params * params);

//generates the shadows from the points on the image.
void distributeSecret(shadowGenerator * shadowGenerator);

//hides the evaluation by using the lbx method.
void hideSecret(shadowGenerator * shadowGenerator);

// function for accessing the bmp files on the directory passed as parameter.
void openDirectory(shadowGenerator * generator, char * directoryPath);

#endif //CRIPTO_SHADOWGENERATOR_H
