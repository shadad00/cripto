//
// Created by shadad on 03/06/23.
//

#ifndef CRIPTO_SHADOWGENERATOR_H
#define CRIPTO_SHADOWGENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Bmp/bmp.h"
#include "../utils.h"
#define P 251
#define Z_p(x) ( (x) < 0 ? (((x)*(-1)*(P-1))%P) : ((x)%P))



typedef struct shadowGenerator{
    bmpFile * file;
    uint8_t  k;
    uint8_t  n;
} shadowGenerator;

typedef struct shadow{
    uint8_t shadowNumber;
    uint8_t * points;
} shadow;

shadowGenerator * initialize(struct params * params);

shadow ** distributeSecret(shadowGenerator * shadowGenerator);

#endif //CRIPTO_SHADOWGENERATOR_H
