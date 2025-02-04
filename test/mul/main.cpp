#include "cases.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <vector>

extern "C" {
#include "../../src/add_sub_mul.h"
#include "../testutils.h"
}

void preprocess_case(size_t case_id) {
  std::vector<uint8_t> &x = cases_x[case_id];
  std::vector<uint8_t> &y = cases_y[case_id];
  // x, y in BE.
  std::reverse(x.begin(), x.end());
  std::reverse(y.begin(), y.end());
  // x, y in LE.
}

void postprocess_case(size_t case_id, std::vector<uint8_t> &result) {
  std::vector<uint8_t> &x = cases_x[case_id];
  std::vector<uint8_t> &y = cases_y[case_id];
  // x, y in LE.
  // z_test in LE.
  std::reverse(x.begin(), x.end());
  std::reverse(y.begin(), y.end());
  // x, y, z in BE.
  // z_test in LE.
  std::reverse(result.begin(), result.end());
  // x, y, z, z_test in BE.
}

void on_bad_rc(size_t case_id, int rc) {
  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");
  printf("Indeterminate test case: %lu.\n", case_id);
  printf("\tError code %d returned.\n", rc);
}

void on_failure(size_t case_id, std::vector<uint8_t> &result) { // Cases data.
  std::vector<uint8_t> x = cases_x[case_id];
  std::vector<uint8_t> y = cases_y[case_id];
  std::vector<uint8_t> z = cases_z[case_id];

  const size_t max_size =
      std::max({x.size(), y.size(), z.size(), result.size()});

  printf("\n");
  printf("Failed test case %d.\n", (int)case_id);
  printf("\tMultiplying x * y\n");
  printf("\t\tx_size  : %lu\n", x.size());
  printf("\t\ty_size  : %lu\n", y.size());
  printf("\t\tz_size  : %lu\n", z.size());
  printf("\t\tx       : ");
  for (int i = 0; i < max_size - x.size(); i++) {
    printf("   ");
  }
  printhex_be(x.data(), x.size() * 8);
  printf("\n");
  printf("\t\ty       : ");
  // Align x and z.
  for (int i = 0; i < max_size - y.size(); i++) {
    printf("   ");
  }
  printhex_be(y.data(), y.size() * 8);
  printf("\n\tResults\n");
  printf("\t\tExpected: ");
  for (int i = 0; i < max_size - z.size(); i++) {
    printf("   ");
  }
  printhex_be(z.data(), z.size() * 8);
  printf("\n");
  printf("\t\tComputed: ");
  for (int i = 0; i < max_size - result.size(); i++) {
    printf("   ");
  }
  printhex_be(result.data(), result.size() * 8);
  printf("\n");
}

int run_testcase_mul_8(size_t case_id, size_t *duration) {
  // Case.
  std::vector<uint8_t> &x = cases_x[case_id];
  std::vector<uint8_t> &y = cases_y[case_id];
  std::vector<uint8_t> &z = cases_z[case_id];
  // Test.
  std::vector<uint8_t> z_test(z.size(), 0);

  // 1. Preprocess
  // 2. Trial
  // 3. Postprocess
  // 4. Report

  preprocess_case(case_id);

  // Start stopclock.
  auto t1 = std::chrono::high_resolution_clock::now();

  uint8_t carry = 0;
  int rc = mul_bstrings_8_gradeschool(x.data(), y.data(), z_test.data(), &carry,
                                      x.size(), y.size(), z_test.size());

  // End stopclock and get duration.
  auto t2 = std::chrono::high_resolution_clock::now();
  *duration =
      (std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() /
       z.size()); // Normalize (ns per byte processed).

  postprocess_case(case_id, z_test);

  if (rc) {
    on_bad_rc(case_id, rc);
    return -1;
  }

  // This won't always work.
  bool success = z == z_test;
  if (!success) {
    on_failure(case_id, z_test);
  }

  return success;
}

void run_all_testcases_mul() {
  const size_t num_cases =
      std::max({cases_x.size(), cases_y.size(), cases_z.size()});

  printf("\n");
  for (int i = 0; i < 72; i++)
    printf("=");
  printf("\n");
  printf("TESTING\n");
  printf("\tFunction: \"mul_bstrings_8_gradeschool\"\n");
  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");

  int passed = 0;
  int failed = 0;
  size_t duration = 0;
  size_t total_duration = 0;
  for (size_t i = 0; i < num_cases; i++) {
    int rc = run_testcase_mul_8(i, &duration);
    total_duration += duration;
    if (rc == 1)
      passed++;
    else if (rc == 0) {
      failed++;
    } else if (rc == 2) {
      // ND.
    }
  }

  size_t avg_duration = total_duration / num_cases;

  for (int i = 0; i < 72; i++)
    printf("-");
  printf("\n");
  printf("RESULTS\n");
  printf("\tPassed: %d / %lu\n", passed, num_cases);
  printf("\tFailed: %d / %lu\n", failed, num_cases);
  printf("\tNdeter: %lu / %lu\n", num_cases - passed - failed, num_cases);
  printf("\n");
  printf("\tAvg. ns per byte processed: %lu\n", avg_duration);
}

int main() {
  run_all_testcases_mul();

  std::vector<uint8_t> x{0x12};
  std::vector<uint8_t> y{0x32};
  std::vector<uint8_t> z(x.size() + y.size());

  for (uint16_t i = 0; i < 256; i++) {
    for (uint16_t j = 0; j < 256; j++) {
      uint16_t res = 0;
      uint8_t flags = 0;
      mul_bstrings_8_gradeschool((uint8_t *)&i, (uint8_t *)&j, (uint8_t *)&res,
                                 &flags, 1, 1, 2);
      uint16_t expected = i * j;
      if (res - expected)
        printf("Failed: %x, %x\n", i, j);
    }
  }
  uint8_t flags = 0;
  mul_bstrings_8_gradeschool(x.data(), y.data(), z.data(), &flags, x.size(),
                             y.size(), z.size());

  printf("z: ");
  printhex(z.data(), 8 * z.size());
  printf("\n");

  return 0;
};
