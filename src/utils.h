//
// Created by shadad on 03/06/23.
//

#ifndef CRIPTO_UTILS_H
#define CRIPTO_UTILS_H
#include <stdio.h>
#include <stdint.h>
#include "./Bmp/bmp.h"
#define SHADOWS_NUMBER 8
#define EXPECTED_PARAMS 5

enum programAction{
    DISTRIBUTE,
    RETRIEVE
};

struct params{
    enum programAction action ;
    char * file;
    uint8_t  k ;
    char * directory;
    uint8_t n;
};


struct params * validateParams(int argc, char * argv[]);

#endif //CRIPTO_UTILS_H
