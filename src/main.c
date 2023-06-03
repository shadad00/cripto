//
// Created by shadad on 01/06/23.
//



#include "utils.h"
#include "./ShadowGenerator/ShadowGenerator.h"


int main(int argc, char * argv[]){
    struct params * params = validateParams(argc, argv);
    shadowGenerator * generator = initialize(params);
    shadow ** list = distributeSecret(generator);

    uint32_t fileSize = generator->file->header->imageSize;
    int blocks = fileSize / (params->k - 1);

    for (int i = 0 ; i < params->n ; i ++){
        printf("bloque: %d", list[i]->shadowNumber);
        for(int j = 0 ; j < blocks; j++)
            printf("%d", list[i]->points[j]);
        printf("----------------------------------------------------\n");
    }

}
