//
// Created by shadad on 01/06/23.
//



#include "utils.h"


int main(int argc, char * argv[]){
    struct params * params = validateParams(argc, argv);
    if (params->action == DISTRIBUTE)
        distribute(params);
    else
        retrieve(params);
}
