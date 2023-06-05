//
// Created by shadad on 03/06/23.
//
#include "utils.h"
#include "./ShadowGenerator/ShadowGenerator.h"
#include "./SecretRetriever/SecretRetriever.h"


uint8_t validK[] = {3,4,5,6,7,8};


struct params * validateParams(int argc, char * argv[]){

    if(argc != EXPECTED_PARAMS){
        exit(-1);
    }
    struct params * params = malloc(sizeof (struct params));
    params->action = strcmp(argv[1], "d") == 0 ? DISTRIBUTE : RETRIEVE;
    params->file = malloc(strlen(argv[2]));
    strcpy(params->file, argv[2]);
    params->k = atoi(argv[3]);
    int valid = 0;
    for(uint8_t i=0; i < sizeof(validK) && !valid; i++){
        if(params->k == validK[i])
            valid = 1;
    }
    if(!valid)
        exit(-2);
    params->directory = malloc(strlen(argv[4]));
    strcpy(params->directory, argv[4]);
    params->n = SHADOWS_NUMBER;
    return params;
}



void distribute(struct params * params){
    shadowGenerator * generator = initialize(params);
    distributeSecret(generator);
    hideSecret(generator);
}


void retrieve(struct params * params ){
    shadowGenerator * generator = initRetriever(params);
    initializeShadows(generator);
    retrieveSecret(generator);
}


uint8_t  sum(uint64_t x , uint64_t y){
    return mod(x+y);
}
uint8_t  sub(uint64_t x , uint64_t y){
    return mod(x-y);
}
uint8_t  mul(uint64_t x , uint64_t y){
    return mod(x*y);
}
uint8_t  modDiv(uint64_t x , uint64_t y){
    return mul(x, inverses[mod(y)]);
}
uint8_t mod(int64_t x){
    while ( x < 0 ){
        x+=P;
    }
    return x % P;
}