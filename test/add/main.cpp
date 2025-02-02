#include "cases.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

extern "C" {
#include "../../src/add_sub_mul.h"
#include "../testutils.h"
}

// g++ -std=c++11 main.c ../testutils.c ../../src/add_sub_mul.c cases.cpp

int run_testcase_add(size_t case_id) {
  // Cases data.
  uint8_t *x = cases_x[case_id].data();
  uint8_t *y = cases_y[case_id].data();
  uint8_t *z = cases_z[case_id].data();
  // Sizes.
  size_t x_size = cases_x[case_id].size();
  size_t y_size = cases_y[case_id].size();
  size_t z_size = cases_z[case_id].size();
  // Experimental.
  size_t z_test_size = z_size; // Could also be a test.

  std::vector<uint8_t> z_test_vec(z_test_size, 0);
  uint8_t *z_test = z_test_vec.data();

  std::reverse(cases_x[case_id].begin(), cases_x[case_id].end());
  std::reverse(cases_y[case_id].begin(), cases_y[case_id].end());
  // x, y in LE.
  // z in BE.

  uint8_t carry = 0;
  int rc = add_bstrings(x, y, z_test, &carry, x_size, y_size, z_test_size);

  std::reverse(cases_x[case_id].begin(), cases_x[case_id].end());
  std::reverse(cases_y[case_id].begin(), cases_y[case_id].end());
  // x, y, z in BE.
  // z_test in LE.

  std::reverse(z_test_vec.begin(), z_test_vec.end());
  // x, y, z, z_test in BE.

  if (rc) {
    for (int i = 0; i < 72; i++)
      printf("-");
    printf("\n");
    printf("Indeterminate test case: %lu.\n", case_id);
    printf("\tError code %d returned.\n", rc);
    return -1;
  }

  // This won't always work.
  bool success = cases_z[case_id] == z_test_vec;

  if (!success) {
    printf("Failed test case %d.\n", (int)case_id);
    printf("\tAdding x + y\n");
    printf("\t\tx       : ");
    for (int i = 0; i < std::max<int>(y_size - x_size, 0); i++)
      printf("   ");
    printhex_be(x, x_size * 8);
    printf("\n");
    printf("\t\ty       : ");
    for (int i = 0; i < std::max<int>(x_size - y_size, 0); i++)
      printf("   ");
    printhex_be(y, y_size * 8);
    printf("\n\tResults\n");
    printf("\t\tExpected: ");
    printhex_be(z, z_size * 8);
    printf("\n");
    printf("\t\tComputed: ");
    printhex_be(z_test, z_test_size * 8);
    printf("\n");
  }

  return success;
}

/**
 * Return codes:
 *    0: Success
 *    1: Failure
 *    2: Indeterminate
 */
int run_testcase_sub(size_t case_id) {
  // x + y = z => x = z - y
  // Cases data.
  uint8_t *x = cases_x[case_id].data();
  uint8_t *y = cases_y[case_id].data();
  uint8_t *z = cases_z[case_id].data();
  // Sizes.
  size_t x_size = cases_x[case_id].size();
  size_t y_size = cases_y[case_id].size();
  size_t z_size = cases_z[case_id].size();

  // Experimental.
  size_t y_test_size = y_size;
  std::vector<uint8_t> y_test_vec(y_test_size, 0);
  uint8_t *y_test = y_test_vec.data();

  std::reverse(cases_x[case_id].begin(), cases_x[case_id].end());
  std::reverse(cases_z[case_id].begin(), cases_z[case_id].end());
  // x, z in LE
  // y in BE

  uint8_t carry = 0;
  int rc = sub_bstrings(z, x, y_test, &carry, z_size, x_size, y_test_size);

  std::reverse(cases_x[case_id].begin(), cases_x[case_id].end());
  std::reverse(cases_z[case_id].begin(), cases_z[case_id].end());
  // x, z, y in BE
  // y_test in LE

  std::reverse(y_test, y_test + y_size);
  // x, z, y, y_test in BE

  if (rc) {
    for (int i = 0; i < 72; i++)
      printf("-");
    printf("\n");
    printf("Indeterminate test case: %lu.\n", case_id);
    printf("\tError code %d returned.\n", rc);
    return -1;
  }

  // This won't always work.
  bool success = cases_y[case_id] == y_test_vec;

  if (!success) {
    printf("\n");
    printf("Failed test case %d.\n", (int)case_id);
    printf("\tSubtracting z - x\n");
    printf("\t\tz       : ");
    printhex_be(z, z_size * 8);
    printf("\n");
    printf("\t\tx       : ");
    // Align x and z.
    for (int i = 0; i < z_size - x_size; i++) {
      printf("   ");
    }
    printhex_be(x, x_size * 8);
    printf("\n\tResults\n");
    printf("\t\tExpected: ");
    for (int i = 0; i < z_size - y_size; i++) {
      printf("   ");
    }
    printhex_be(y, y_size * 8);
    printf("\n");
    printf("\t\tComputed: ");
    for (int i = 0; i < z_size - y_test_size; i++) {
      printf("   ");
    }
    printhex_be(y_test, y_test_size * 8);
    printf("\n");
  }

  return success;
}

void run_all_testcases_add() {
  const size_t num_cases =
      std::max({cases_x.size(), cases_y.size(), cases_z.size()});

  printf("\n");
  for (int i = 0; i < 72; i++)
    printf("=");
  printf("\n");
  printf("TESTING\n");
  printf("\tFunction: \"add_bstrings\"\n");
  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");

  int passed = 0;
  int failed = 0;
  for (size_t i = 0; i < num_cases; i++) {
    int rc = run_testcase_add(i);
    if (rc == 1)
      passed++;
    else if (rc == 0) {
      failed++;
    } else if (rc == 2) {
      // ND.
    }
  }

  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");
  printf("RESULTS\n");
  printf("\tPassed: %d / %lu\n", passed, num_cases);
  printf("\tFailed: %d / %lu\n", failed, num_cases);
  printf("\tNdeter: %lu / %lu\n", num_cases - passed - failed, num_cases);
}
void run_all_testcases_sub() {
  const size_t num_cases =
      std::max({cases_x.size(), cases_y.size(), cases_z.size()});

  printf("\n");
  for (int i = 0; i < 72; i++)
    printf("=");
  printf("\n");
  printf("TESTING\n");
  printf("\tFunction: \"sub_bstrings\"\n");
  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");

  int passed = 0;
  int failed = 0;
  for (size_t i = 0; i < num_cases; i++) {
    int rc = run_testcase_sub(i);
    if (rc == 1)
      passed++;
    else if (rc == 0) {
      failed++;
    } else if (rc == 2) {
      // ND.
    }
  }

  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");
  printf("RESULTS\n");
  printf("\tPassed: %d / %lu\n", passed, num_cases);
  printf("\tFailed: %d / %lu\n", failed, num_cases);
  printf("\tNdeter: %lu / %lu\n", num_cases - passed - failed, num_cases);
}

int main() {
  run_all_testcases_add();
  run_all_testcases_sub();
  return 0;
};
