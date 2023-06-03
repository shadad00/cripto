//
// Created by shadad on 03/06/23.
//
#include "ShadowGenerator.h"

static  shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint32_t shadowPoints);
static uint8_t evaluatePolynomial(shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value);

shadowGenerator * initialize(struct params * params){
    struct shadowGenerator * shadowGenerator = malloc(sizeof(shadowGenerator));
    shadowGenerator->file = openBmpFile(params->file);
    shadowGenerator->k = params->k;
    shadowGenerator->n = params -> n;
    return shadowGenerator;
}

void distributeSecret(shadowGenerator * shadowGenerator){
    uint32_t shadowPoints = (shadowGenerator->file->header->imageSize) / (shadowGenerator->k - 1);
    struct shadow ** shadowArray  = initializeShadowArray(shadowGenerator, shadowPoints);

    uint8_t * pixelPoints = shadowGenerator->file->pixels;
    uint8_t k = shadowGenerator->k;
    uint32_t currentBlock = 0;
    uint8_t * aCoefficients = malloc( k * sizeof (uint8_t));
    uint8_t * bCoefficients= malloc( k * sizeof (uint8_t));

    srand(time(NULL)); //initialize the seed.


    while(currentBlock < shadowPoints){

        memcpy(aCoefficients, pixelPoints, k);
        memcpy(bCoefficients + 2, pixelPoints + k, k-2 );
        uint8_t random = rand() + 1 ;  //avoid the random number to be zero.
        a_0 = aCoefficients[0] == 0 ? 1: aCoefficients[0];
        a_1 = aCoefficients[1] == 0 ? 1: aCoefficients[1];
        bCoefficients[0] =  Z_p(- random * a_0);
        bCoefficients[1] =  Z_p(- random * a_1);

        for (int j = 0; j < shadowGenerator->n; j++){
            shadowArray[j]->points[currentBlock] = evaluatePolynomial(shadowGenerator,
                                                                      aCoefficients,shadowArray[j]->shadowNumber);
            shadowArray[j]->points[currentBlock + 1] = evaluatePolynomial(shadowGenerator,
                                                                          bCoefficients, shadowArray[j]->shadowNumber);
        }

        pixelPoints += (2 * k) - 2;
        currentBlock +=2;
    }

    shadowGenerator->generatedShadows =  shadowArray;
}


static shadow ** initializeShadowArray(shadowGenerator * shadowGenerator, uint32_t shadowPoints){

     shadow ** shadowArray = malloc(shadowGenerator->n * sizeof(shadow *));
    //initialize all shadow structures.
    for (int i = 0; i < shadowGenerator -> n ; i++){
        shadowArray[i] = malloc(sizeof(struct shadow));
        shadowArray[i]->shadowNumber = i + 1;
        shadowArray[i]->points = malloc(shadowPoints * sizeof (uint8_t));
    }
    return  shadowArray;
}

static uint8_t evaluatePolynomial(struct shadowGenerator * shadowGenerator, uint8_t * coefficients, uint8_t value){
    int x = 1;
    int evaluation = 0;
    for(int i = shadowGenerator->k - 1 ; i >= 0   ; i--){
        evaluation += coefficients[i] * x ;
        x *= value;
    }
    return Z_p(evaluation);
}

