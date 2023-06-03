//
// Created by shadad on 03/06/23.
//
#include "utils.h"

uint8_t validK[] = {3,4,5,6,7,8};


struct params * validateParams(int argc, char * argv[]){

    if(argc != EXPECTED_PARAMS){
        exit(-1);
    }
    struct params * params = malloc(sizeof (struct params));
    params->action = strcmp(argv[1], "d")? DISTRIBUTE : RETRIEVE;
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