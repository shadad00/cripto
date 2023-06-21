//
// Created by shadad on 03/06/23.
//
#include "utils.h"
#include "./ShadowGenerator/ShadowGenerator.h"
#include "./SecretRetriever/SecretRetriever.h"



struct params * validateParams(int argc, char * argv[]){
    if(argc != EXPECTED_PARAMS){
        printf("The provided parameters are incorrect. \n");
        exit(EXIT_FAILURE);
    }
    struct params * params = malloc(sizeof (struct params));

    params->action = strcmp(argv[1], "d") == 0 ? DISTRIBUTE : RETRIEVE;

    params->file = malloc(strlen(argv[2]) + 1);
    strcpy(params->file, argv[2]);

    params->k = atoi(argv[3]);
    if ( params->k < 3 || params->k > 8){
        printf("The value given to k is not correct. \n");
        exit(EXIT_FAILURE);
    }

    params->directory = malloc(strlen(argv[4]) + 1 );
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


