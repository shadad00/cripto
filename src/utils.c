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
    struct params * receivedParams = malloc(sizeof (struct params));

    receivedParams->action = strcmp(argv[1], "d") == 0 ? DISTRIBUTE : RETRIEVE;

    receivedParams->file = malloc(strlen(argv[2]) + 1);
    strcpy(receivedParams->file, argv[2]);

    receivedParams->k = atoi(argv[3]);
    if (receivedParams->k < 3 || receivedParams->k > 8){
        printf("The value given to k is not correct. \n");
        exit(EXIT_FAILURE);
    }

    receivedParams->directory = malloc(strlen(argv[4]) + 1 );
    strcpy(receivedParams->directory, argv[4]);

    receivedParams->n = SHADOWS_NUMBER;

    return receivedParams;
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


