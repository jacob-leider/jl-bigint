#ifndef __JL_ADD_SUB_MUL_H__
#define __JL_ADD_SUB_MUL_H__

#include <stdint.h>
#include <stdio.h>

uint8_t add_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *carry, size_t x_size, size_t y_size,
                     size_t z_size);

uint8_t sub_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *carry, size_t x_size, size_t y_size,
                     size_t z_size);

uint8_t sub_bstrings_nocheck(const uint8_t *x, const uint8_t *y, uint8_t *z,
                             size_t x_size, size_t y_size, size_t z_size);

uint8_t add_bstrings_nocheck(const uint8_t *x, const uint8_t *y, uint8_t *z,
                             size_t x_size, size_t y_size, size_t z_size);
#endif
