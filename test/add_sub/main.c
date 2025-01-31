#include "../../src/add_sub_mul.h"
#include "../testutils.h"
#include "cases.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

extern "C" {
#include "../../src/add_sub_mul.h"
}

int run_testcase_sub(size_t case_id) {
  // x + y = z => x = z - y
  // Cases data.
  uint8_t *x = cases_x[case_id].data();
  uint8_t *z = cases_z[case_id].data();
  uint8_t *y_expected = cases_y[case_id].data();
  // Sizes.
  size_t x_size = cases_x[case_id].size();
  size_t z_size = cases_z[case_id].size();
  size_t y_size = cases_y[case_id].size();
  // Experimental.
  size_t output_size = std::max(x_size, y_size);
  std::vector<uint8_t> y_test_vec(output_size, 0);
  uint8_t *y_test = y_test_vec.data();

  std::reverse(x, x + x_size);
  std::reverse(z, z + z_size);

  uint8_t carry = 0;
  sub_bstrings(z, x, y_test, &carry, z_size, x_size, output_size);

  std::reverse(x, x + x_size);
  std::reverse(z, z + z_size);
  std::reverse(y_test, y_test + y_size);

  // This won't always work.
  bool success = cases_y[case_id] == y_test_vec;

  if (!success) {
    printf("Failed test case %d.\n", (int)case_id);
    printf("Expected: ");
    printhex(y_expected, z_size * 8);
    printf("\n");
    printf("Computed: ");
    printhex(y_test, output_size * 8);
    printf("\n");
  }

  return success;
}

int run_testcase_add(size_t case_id) {
  // Cases data.
  uint8_t *x = cases_x[case_id].data();
  uint8_t *y = cases_y[case_id].data();
  uint8_t *z_expected = cases_z[case_id].data();
  // Sizes.
  size_t x_size = cases_x[case_id].size();
  size_t y_size = cases_y[case_id].size();
  size_t z_size = cases_z[case_id].size();
  // Experimental.
  size_t output_size = std::max(x_size, y_size);
  std::vector<uint8_t> z_test_vec(output_size, 0);
  uint8_t *z_test = z_test_vec.data();

  std::reverse(x, x + x_size);
  std::reverse(y, y + y_size);

  uint8_t carry = 0;
  add_bstrings(x, y, z_test, &carry, x_size, y_size, output_size);

  std::reverse(x, x + x_size);
  std::reverse(y, y + y_size);
  std::reverse(z_test, z_test + z_size);

  // This won't always work.
  bool success = cases_z[case_id] == z_test_vec;

  if (!success) {
    printf("Failed test case %d.\n", (int)case_id);
    printf("Expected: ");
    printhex(z_expected, z_size * 8);
    printf("\n");
    printf("Computed: ");
    printhex(z_test, output_size * 8);
    printf("\n");
  }

  return success;
}

void run_tests() {
  const size_t num_cases =
      std::max({cases_x.size(), cases_y.size(), cases_z.size()});

  // TEST ADD
  int passed = 0;
  int failed = 0;
  for (size_t i = 0; i < num_cases; i++) {
    if (run_testcase_add(i))
      passed++;
    else
      failed++;
  }

  printf("TESTING: \"add_bstrings\"\n");
  printf("\tTest cases passed: %d / %d\n", passed, passed + failed);

  // TEST SUB
  passed = 0;
  failed = 0;
  for (size_t i = 0; i < num_cases; i++) {
    if (run_testcase_sub(i))
      passed++;
    else
      failed++;
  }

  printf("TESTING: \"sub_bstrings\"\n");
  printf("\tTest cases passed: %d / %d\n", passed, passed + failed);
}

int main() {
  run_tests();
  return 0;
};
