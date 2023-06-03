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
#define P 251
#define Z_p(x) ( (x) < 0 ? (((x)*(-1)*(P-1))%P) : ((x)%P))


typedef struct shadow{
    uint8_t shadowNumber;
    uint32_t pointNumber;
    uint8_t * points;
} shadow;

typedef struct shadowGenerator{
    bmpFile * file;
    uint8_t  k;
    uint8_t  n;
    shadow ** generatedShadows;
    char ** imageFiles;
} shadowGenerator;

shadowGenerator * initialize(struct params * params);

void distributeSecret(shadowGenerator * shadowGenerator);

void hideSecret(shadowGenerator * shadowGenerator);

#endif //CRIPTO_SHADOWGENERATOR_H
