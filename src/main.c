//
// Created by shadad on 01/06/23.
//
#include <stdio.h>
#include "./Bmp/bmp.h"
#include <string.h>
#include <stdint.h>

#define EXPECTED_PARAMS 5

uint8_t validK[] = {3,4,5,6,7,8};


enum programAction{
    DISTRIBUTE,
    RETRIEVE
};

struct params{
    enum programAction action ;
    char * file;
    uint8_t  k ;
    char * directory;
};

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
    return params;
}



int main(int argc, char * argv[]){
    struct params * params = validateParams(argc, argv);
    printf("hola%d", params->k);
    bmpFile  * ptr = openBmpFile("albert.bmp");
    uint8_t * readptr = ptr ->pixels;
    printf("El tamaño es: %d \n", ptr->header->fileSize);
    for (uint32_t i = 0; i < ptr->header->fileSize; i++)
        printf("%d",readptr[i]);

    printf("Finished");
}
