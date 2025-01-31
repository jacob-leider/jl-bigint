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
static inline void add_block(uint8_t x, uint8_t y, uint8_t *z, uint8_t *carry,
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
static inline void sub_block(uint8_t x, uint8_t y, uint8_t *z, uint8_t *carry,
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

inline uint8_t add_bstrings_nocheck(const uint8_t *x, const uint8_t *y,
                                    uint8_t *z, size_t x_size, size_t y_size,
                                    size_t z_size) {
  uint8_t carry = 0;
  for (int i = 0; i < x_size; i++) {
    add_block(x[i], y[i], z, &carry, i);
  }

  for (int i = x_size; i < y_size; i++) {
    add_block(0, y[i], z, &carry, i);
  }

  return carry;
}

/**
 * @brief Subtracts @p y and @p x, stores the difference in @p z.
 */
static inline uint8_t sub_bstrings_nocheck(const uint8_t *x, const uint8_t *y,
                                           uint8_t *z, size_t x_size,
                                           size_t y_size, size_t z_size) {
  // Subtract y from x.
  uint8_t carry = 0;

  for (int i = 0; i < y_size; i++) {
    sub_block(x[i], y[i], z, &carry, i);
  }

  for (int i = y_size; i < x_size && carry != 0; i++) {
    sub_block(x[i], 0, z, &carry, i);
  }

  return carry;
}

/**
 * @brief Add @p x and @p y, store in @p z.
 *
 * Error codes:
 *    (0) Success.
 *    (1) @p x, @p y, or @p z is NULL.
 *    (2) max(@p x_size, @p y_size) > @p z_size
 *
 * @param[in] x (uint8_t*): Address of least significant byte of @p x.
 * @param[in] y (uint8_t*): Address of least significant byte of @p y.
 * @param[out] z (uint8_t*): Address of least significant byte of @p z.
 * @param[out] carry (uint8_t*): 1 if @p x + @p y > 2^(max(@p x_size, @p
 * y_size)), 0 otherwise.
 * @param x_size[in] (size_t): Size of @p x.
 * @param y_size[in] (size_t): Size of @p y.
 * @param z_size[in] (size_t): Size of @p z.
 *
 * @return ret (uint8_t): An error code.
 */
uint8_t add_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *carry, size_t x_size, size_t y_size,
                     size_t z_size) {
  // Error check 1.
  if (x == NULL | y == NULL | z == NULL)
    return 1;
  // Error check 2.
  if ((x_size > z_size) | (y_size > z_size))
    return 2;

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

  uint8_t carry_temp = add_bstrings_nocheck(x, y, z, x_size, y_size, z_size);
  *carry = carry_temp;

  return 0;
}

/**
 * @brief Subtract y from @p x, store in @p z. the size of y may not be larger
 * than the size of @p x. If @p x < @p y, the value stored in @p z is 2^(@p
 * x_size + 1) + @p x - @p y, and carry is nonzero. If @p x >= @p y, the value
 * stored in @p z is @p x - @p y.
 *
 * Error codes:
 *    (0) Success.
 *    (1) @p x, @p y, or @p z is NULL.
 *    (2) max(@p x_size, @p y_size) > @p z_size
 *    (3) @p x_size < @p y_size
 *
 * @param[in] x (uint8_t*): Address of least significant byte of @p x.
 * @param[in] y (uint8_t*): Address of least significant byte of @p y.
 * @param[out] z (uint8_t*): Address of least significant byte of @p z.
 * @param[out] carry (uint8_t*): The difference (@p x - @p y) - @p z between the
 * number stored in @p z, and the (potentially negative) number @p x - @p y.
 * @param x_size[in] (size_t): Size of @p x.
 * @param y_size[in] (size_t): Size of @p y.
 * @param z_size[in] (size_t): Size of @p z.
 *
 * @return ret (uint8_t): An error code.
 */

/**
 * @brief Adds @p x and @p y, stores the sum in @p z.
 *  Assumes @p y_size larger than @p x_size.
 */
uint8_t sub_bstrings(const uint8_t *x, const uint8_t *y, uint8_t *z,
                     uint8_t *carry, size_t x_size, size_t y_size,
                     size_t z_size) {
  // Error check 1.
  if (x == NULL | y == NULL | z == NULL)
    return 1;
  // Error check 2.
  if (x_size > z_size | y_size > z_size)
    return 2;
  // Error check 3.
  if (x_size < y_size)
    return 3;

  uint8_t carry_temp = sub_bstrings_nocheck(x, y, z, x_size, y_size, z_size);
  *carry = carry_temp;

  return 0;
}
