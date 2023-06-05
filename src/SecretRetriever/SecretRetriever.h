//
// Created by shadad on 03/06/23.
//

#ifndef CRIPTO_SECRETRETRIEVEN_H
#define CRIPTO_SECRETRETRIEVEN_H
#include "../ShadowGenerator/ShadowGenerator.h"


shadowGenerator * initRetriever(struct params * params);

void initializeShadows(shadowGenerator * generator);
void retrieveSecret(shadowGenerator * generator) ;

#endif //CRIPTO_SECRETRETRIEVEN_H
