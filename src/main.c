//
// Created by shadad on 01/06/23.
//



#include "utils.h"
#include "./ShadowGenerator/ShadowGenerator.h"


int main(int argc, char * argv[]){
    struct params * params = validateParams(argc, argv);
    shadowGenerator * generator = initialize(params);
    distributeSecret(generator);
}
