#include <stdio.h>
#include <stdlib.h>

#ifndef __JL_TESTUTILS_H__
#define __JL_TESTUTILS_H__

int check_endianness();

void printhex_le(const uint8_t *x, int num_bits);

void printhex_be(const uint8_t *x, int num_bits);

void printhex(const uint8_t *x, int num_bits);

void printbin_le(const uint8_t *x, int num_bits);

void printbin_be(const uint8_t *x, int num_bits);

void printbin(const uint8_t *x, int num_bits);

#endif
