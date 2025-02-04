#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Perform addition of 8 bit unsigned ints, using only 8 bit unsigned
 * ints.
 *
 * @remark It can be shown that cases 1, 2 and 3 are mutually exclusive and
 * collectively exhaustive.
 *
 * @param[in] x (uint8_t): Summand.
 * @param[in] y (uint8_t): Summand.
 * @param[out] z (uint8_t*): Should be empty. Stores @p x + @p y + @p carry if
 * less than 256, @p x + @p y + @p - 256 otherwise.
 * @param[out] carry (uint8_t): Set to 1 if @p x + @p y + @p carry > 255. Must
 * be one or zero.
 * @param[in] i (size_t): offset from @p z.
 */
static void add_block(uint8_t x, uint8_t y, uint8_t *z, uint8_t *carry,
                      size_t i) {
  // Case 1: x + y == 255 and carry == 1.
  if (y == 0xFF - x) {
    if (*carry) {
      z[i] = 0;
      *carry = 1;
      return;
    }
  }

  // Case 2: x + y > 255 and 0 <= carry <= 1.
  if (y > 0xFF - x) {
    z[i] = (y - (0xFF - x)) + *carry - 1;
    *carry = 1;
    return;
  }

  // Case 3: x + y + carry <= 255.
  z[i] = x + y + *carry;
  *carry = 0;
}

/**
 * @brief Perform subtraction of 8 bit unsigned ints, using only 8 bit unsigned
 * ints.
 *
 * @remark It can be shown that cases 1, 2 and 3 are mutually exclusive and
 * collectively exhaustive.
 *
 * @param[in] x (uint8_t): From which @p y will be subtracted.
 * @param[in] y (uint8_t): To be subtracted from @p x.
 * @param[out] z (uint8_t*): Should be empty. Stores @p x - @p y - @p carry if
 * nonnegative, 256 + @p x - @p y - @p otherwise.
 * @param[out] carry (uint8_t): Set to 1 if @p x - @p y - @p carry < 0. Must be
 * one or zero.
 * @param[in] i (size_t): offset from @p z.
 */
static void sub_block(uint8_t x, uint8_t y, uint8_t *z, uint8_t *carry,
                      size_t i) {
  // Case 1: x - y - carry == -1;
  if (x == y) {
    if (*carry == 1) {
      z[i] = 0xFF;
      return; // Carry stays 1.
    }
  }

  // Case 2: x - y < 0
  if (y > x) {
    z[i] = (((0xFF - y) + x) - *carry) + 1;
    *carry = 1;
    return;
  }

  // Case 3: x - y - carry >= 0
  z[i] = x - y - *carry;
  *carry = 0;
}

/**
 * @brief Perform multiplication of 8 bit unsigned ints, using only 8 bit
 * unsigned ints.
 *
 * @param[in] x (uint8_t): Factor.
 * @param[in] y (uint8_t): Factor.
 * @param[out] z (uint8_t*): Must be two bytes. Will store the LSBS in the first
 * byte, and MSBS in the second byte.
 */
static void mult_block(uint8_t x, uint8_t y, uint8_t *z) {
  const uint8_t x0 = x & 0x0F; // last 4 bits of x
  const uint8_t x1 = x >> 4;   // first 4 bits of x
  const uint8_t y0 = y & 0x0F; // last 4 bits of y
  const uint8_t y1 = y >> 4;   // first 4 bits of y
  uint8_t z0 = x0 * y0;
  uint8_t z1 = 0;
  uint8_t z2 = x1 * y1;
  uint8_t c = 0;

  add_block(x0 * y1, x1 * y0, &z1, &c, 0); // Compute z1.

  if (c)
    z2 += (1 << 4);

  c = 0;

  // Compute temp 0 (LS 8 bits of prod).
  add_block(z0, (z1 << 4), z, &c, 0);

  if (c)
    z2 += 1;

  c = 0;

  // Compute temp 1 (MS 8 bits of prod).
  add_block(z2, (z1 >> 4), z, &c, 1);
}

// x_size <= y_size <= z_size
static uint8_t sub_bstrings_nocheck_xyz(const uint8_t *x, const uint8_t *y,
                                        uint8_t *z, size_t x_size,
                                        size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < x_size; i++)
    sub_block(x[i], y[i], z, &carry, i);
  // x[i] is zero
  for (int i = x_size; i < y_size; i++)
    sub_block(0, y[i], z, &carry, i);
  // x[i] and y[i] are zero
  for (int i = y_size; i < z_size; i++)
    sub_block(0, 0, z, &carry, i);

  return carry;
}

// x_size <= z_size <= y_size
static uint8_t sub_bstrings_nocheck_xzy(const uint8_t *x, const uint8_t *y,
                                        uint8_t *z, size_t x_size,
                                        size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < x_size; i++)
    sub_block(x[i], y[i], z, &carry, i);
  // x[i] is zero
  for (int i = x_size; i < z_size; i++)
    sub_block(0, y[i], z, &carry, i);

  return carry;
}

// y_size <= x_size <= z_size
static uint8_t sub_bstrings_nocheck_yxz(const uint8_t *x, const uint8_t *y,
                                        uint8_t *z, size_t x_size,
                                        size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < y_size; i++)
    sub_block(x[i], y[i], z, &carry, i);
  // y[i] is zero
  for (int i = y_size; i < x_size; i++)
    sub_block(x[i], 0, z, &carry, i);
  // y[i] and x[i] are zero
  for (int i = x_size; i < z_size; i++)
    sub_block(0, 0, z, &carry, i);

  return carry;
}

// y_size <= z_size <= x_size
static uint8_t sub_bstrings_nocheck_yzx(const uint8_t *x, const uint8_t *y,
                                        uint8_t *z, size_t x_size,
                                        size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < y_size; i++)
    sub_block(x[i], y[i], z, &carry, i);
  // y[i] is zero.
  for (int i = y_size; i < z_size; i++)
    sub_block(x[i], 0, z, &carry, i);

  return carry;
}

// z_size <= x_size, y_size
static uint8_t sub_bstrings_nocheck_zxy(const uint8_t *x, const uint8_t *y,
                                        uint8_t *z, size_t x_size,
                                        size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < z_size; i++)
    sub_block(x[i], y[i], z, &carry, i);

  return carry;
}

/**
 * @brief Adds @p x and @p y and stores their sum in @p z without performing any
 * error handling.
 *
 * Requires y_size <= x_size, and that x, y, and z are not null.
 */
uint8_t add_bstrings_nocheck(const uint8_t *x, const uint8_t *y, uint8_t *z,
                             size_t x_size, size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < y_size && i < z_size; i++)
    add_block(x[i], y[i], z, &carry, i);
  // y[i] is zero.
  for (int i = y_size; i < x_size && i < z_size; i++)
    add_block(x[i], 0, z, &carry, i);

  // If we have room for overflow.
  if (x_size < z_size && carry != 0) {
    z[x_size] = 1;
    carry = 0;
    // Set overflow bit.
    carry |= 0x02;
  }

  return carry;
}

/**
 * @brief Subtracts @p y and @p x and stores the difference in @p z without
 * performing any error handling. @p carry will be set to 1 if the result is
 * negative or @p z_size doesn't fit the difference @p y - @- x.
 *
 * Let dx and dy be the orders of the most significant bits of x, y. Let dz
 * be z_size - 1.
 *
 *    e.g. 2^(dx - 1) < x <= 2^dx.
 *
 * (1) If y > x: sub(x, y, z) returns (y - x) mod 2^z_size.
 *
 *    Note: (y - x) mod 2^dz can now be obtained as
 *          2^dz - sub(x, y, z).
 *    Note: As integers, x - y can be obtained as sub(x, y, z) - 2^dz
 *
 * (2) If y < x: sub(x, y, z) = (x - y) mod 2^dz.
 *
 *    If dz >= d(x - y): sub(x, y, z) = x - y.
 *
 *    If dz < d(x - y): z will miss leading bits, and the carry flag will be
 *                      set.
 *
 * (3) If y == x: sub(x, y, z) = 0, and always fits in z.
 *
 * Requires that @p x, @p y, and @p z are not null.
 */
uint8_t sub_bstrings_nocheck(const uint8_t *x, const uint8_t *y, uint8_t *z,
                             size_t x_size, size_t y_size, size_t z_size) {
  uint8_t carry = 0;
  if (x_size <= y_size) {
    if (z_size <= x_size) {
      // z_size <= x_size <= y_size
      return sub_bstrings_nocheck_zxy(x, y, z, x_size, y_size, z_size);
    } else if (z_size <= y_size) {
      // x_size < z_size <= y_size
      return sub_bstrings_nocheck_xzy(x, y, z, x_size, y_size, z_size);
    } else {
      // x_size < y_size <= z_size
      return sub_bstrings_nocheck_xyz(x, y, z, x_size, y_size, z_size);
    }
  } else {
    if (z_size <= y_size) {
      // z_size <= y_size < x_size
      return sub_bstrings_nocheck_zxy(x, y, z, x_size, y_size, z_size);
    } else if (z_size <= x_size) {
      // y_size < z_size <= x_size
      return sub_bstrings_nocheck_yzx(x, y, z, x_size, y_size, z_size);
    } else {
      // y_size < x_size < z_size
      return sub_bstrings_nocheck_yxz(x, y, z, x_size, y_size, z_size);
    }
  }

  return carry;
}

uint8_t mul_bstrings_8_gradeschool_nocheck(const uint8_t *x, const uint8_t *y,
                                           uint8_t *z, size_t x_size,
                                           size_t y_size, size_t z_size) {
  uint8_t temp[3] = {0, 0, 0}; // prod LSBs, prod MSBs, flag byte
  for (size_t i = 0; i < x_size; i++) {
    for (size_t j = 0; j < y_size; j++) {
      if (x[i] | y[j]) {
        mult_block(x[i], y[j], temp);
        add_bstrings(z + i + j, temp, z + i + j, temp + 2, z_size - i - j, 2,
                     z_size - i - j);
      }
    }
  }

  return 0;
}

/**
 * @brief Adds @p x and @p y, and stores the sum in @p z.
 *
 *  - Error codes:
 *      0. Success.
 *      1. @p x, @p y, or @p z is NULL.
 *
 *  - Flags (bit index, significance increasing):
 *      0. Carry bit. Set if the last carried value is 1. Implies @p x +
 *        @p y doesn't fit in @p z, but the converse doesn't necessarily hold.
 *      1. Overflow bit. Set if @p x + @p y has a larger  binary representation
 *        than @p x or @p y. Meaningless if the 0-bit is set.
 *
 * @param[in] x (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[in] y (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[out] z (uint8_t*):  On return, @p z stores the sum @p x + @p y in
 * little-endian order. If @p z is larger than the minimum memory required to
 * store @p x + @p y, significant bits are assumed to be zero. If @p z is
 * smaller than the minimum memory required to store @p x + @p y, @p z stores
 * the first @ z_size bytes of @p x + @p y ordered by increasing significance.
 * @param[out] carry (uint8_t*): 1 if @p x + @p y > 2^(max(@p x_size, @p
 * y_size)), 0 otherwise.
 * @param[in] x_size (size_t): Size of @p x.
 * @param[in] y_size (size_t): Size of @p y.
 * @param[in] z_size (size_t): Size of @p z.
 *
 * @return (uint8_t): An error code. See description for details.
 */
uint8_t add_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *flags, size_t x_size, size_t y_size,
                     size_t z_size) {
  // Error check 1.
  if (x == NULL | y == NULL | z == NULL)
    return 1;

  // Require x be larger than y.
  if (x_size < y_size) {
    // Swap pointers.
    {
      const uint8_t *temp = x;
      x = y;
      y = temp;
    }
    // Swap sizes.
    {
      size_t temp = x_size;
      x_size = y_size;
      y_size = temp;
    }
  }

  *flags = add_bstrings_nocheck(x, y, z, x_size, y_size, z_size);

  return 0;
}

/**
 * @brief Subtract y from @p x, store in @p z. the size of y may not be larger
 * than the size of @p x. If @p x < @p y, the value stored in @p z is 2^(@p
 * x_size + 1) + @p x - @p y, and carry is nonzero. If @p x >= @p y, the value
 * stored in @p z is @p x - @p y.
 *
 *  - Error codes:
 *      0. Success.
 *      1. @p x, @p y, or @p z is NULL.
 *
 *  - Flags:
 *      0. Carry bit is set.
 *
 * @param[in] x (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[in] y (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[out] z (uint8_t*): Address of least significant byte of @p z.
 * @param[out] carry (uint8_t*): The difference (@p x - @p y) - @p z between
 * the number stored in @p z, and the (potentially negative) number @p x - @p
 * y.
 * @param x_size[in] (size_t): Size of @p x.
 * @param y_size[in] (size_t): Size of @p y.
 * @param z_size[in] (size_t): Size of @p z.
 *
 * @return (uint8_t): An error code.
 */
uint8_t sub_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *flags, size_t x_size, size_t y_size,
                     size_t z_size) {
  // Error check 1.
  if (x == NULL | y == NULL | z == NULL)
    return 1;

  *flags = sub_bstrings_nocheck(x, y, z, x_size, y_size, z_size);

  return 0;
}

/**
 * @brief Multiplies x and y, and stores the product in @p z. If @p z_size is
 * smaller than the binary representation of @p x * @p y, @p z will store the
 * least significant @p z_size bytes of the product.
 *
 *  - Error codes:
 *      0. Success.
 *      1. @p x, @p y, or @p z is NULL.
 *
 * @param[in] x (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[in] y (uint8_t*): Points to an array of bytes in little-endian
 * order.
 * @param[out] z (uint8_t*): Address of least significant byte of @p z.
 * @param x_size[in] (size_t): Size of @p x.
 * @param y_size[in] (size_t): Size of @p y.
 * @param z_size[in] (size_t): Size of @p z.
 *
 * @return An error code. See Error codes in the description.
 */
uint8_t mul_bstrings_8_gradeschool(const uint8_t *x, const uint8_t *y,
                                   uint8_t *z, uint8_t *flags, size_t x_size,
                                   size_t y_size, size_t z_size) {
  // Error check 1.
  if (x == NULL | y == NULL | z == NULL)
    return 1;

  *flags = mul_bstrings_8_gradeschool_nocheck(x, y, z, x_size, y_size, z_size);

  return 0;
}
