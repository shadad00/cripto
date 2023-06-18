//
// Created by shadad on 18/06/23.
//

#ifndef CRIPTO_MODULAROPERATIONS_H
#define CRIPTO_MODULAROPERATIONS_H
#include <stdio.h>
#include <stdint.h>
#define P 251


uint8_t  sum(uint64_t x , uint64_t y);
uint8_t  sub(uint64_t x , uint64_t y);
uint8_t  mul(uint64_t x , uint64_t y);
uint8_t  modDiv(uint64_t x , uint64_t y);
uint8_t mod(int64_t x);


#endif //CRIPTO_MODULAROPERATIONS_H
