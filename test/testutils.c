#ifndef __JL_TESTUTILS_H__
#define __JL_TESTUTILS_H__

#include <stdint.h>
#include <stdio.h>

/**
 * @return endianness: 0 for little endian, 1 for big endian.
 */
int check_endianness() {
  int num = 1;
  return *((char *)&num) == 0;
}

void printhex_le(const uint8_t *x, int num_bits) {
  for (int j = num_bits / 8 - 1; j >= 0; j--) {
    printf("%X", (x[j] >> 4));
    printf("%X", (x[j] & 0x0F));
    printf(" ");
  }
}

void printhex_be(const uint8_t *x, int num_bits) {
  for (int j = 0; j < num_bits / 8; j++) {
    printf("%X", (x[j] >> 4));
    printf("%X", (x[j] & 0x0F));
    printf(" ");
  }
}

void printhex(const uint8_t *x, int num_bits) {
  if (check_endianness())
    printhex_be(x, num_bits);
  else
    printhex_le(x, num_bits);
}

void printbin_le(const uint8_t *x, int num_bits) {
  for (int j = num_bits / 8 - 1; j >= 0; j--) {
    for (int i = 0; i < sizeof(uint8_t) * 8; i++) {
      if (i == 4)
        printf(" ");
      if (x[j] & (1 << (7 - i)))
        printf("1");
      else
        printf("0");
    }
    printf(" ");
  }
}

void printbin_be(const uint8_t *x, int num_bits) {
  for (int j = 0; j < num_bits / 8; j++) {
    for (int i = 0; i < sizeof(uint8_t) * 8; i++) {
      if (i == 4)
        printf(" ");
      if (x[j] & (1 << (7 - i)))
        printf("1");
      else
        printf("0");
    }
    printf(" ");
  }
}

void printbin(const uint8_t *x, int num_bits) {
  if (check_endianness())
    printbin_be(x, num_bits);
  else
    printbin_le(x, num_bits);
  return;
}

#endif
