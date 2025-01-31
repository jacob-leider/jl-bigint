#!/usr/bin/env python3

# Run this in its directory to generate test cases.

import random
import os

def generate_cfile() -> str:
    c1 = []
    c2 = []
    c3 = []

    for i in range(10):
      r1 = random.randint(2**100, 2**101)
      r2 = random.randint(2**100, 2**101)
      r3 = r1 + r2

      s1 = f"{r1:X}"
      s2 = f"{r2:X}"
      s3 = f"{r3:X}"

      l1 = [s1[i:i + 1] + s1[i + 1:i + 2] for i in range(0,len(s1),2)]
      l2 = [s2[i:i + 1] + s2[i + 1:i + 2] for i in range(0,len(s2),2)]
      l3 = [s3[i:i + 1] + s3[i + 1:i + 2] for i in range(0,len(s3),2)]

      c1.append(f"{'{'}0x{', 0x'.join(l1)}{'}'}")
      c2.append(f"{'{'}0x{', 0x'.join(l2)}{'}'}")
      c3.append(f"{'{'}0x{', 0x'.join(l3)}{'}'}")

    headers = ["vector"]
    local_headers = [h_file_name]

    local_header_str = '\n'.join([f"#include\"{lh}\"" for lh in local_headers])
    header_str = '\n'.join([f"#include<{h}>" for h in headers])

    casetype = "std::vector<std::vector<uint8_t>>"

    cl1 = ',\n'.join(c1)
    o1 = f"{casetype} cases_x = {'{'}{cl1}{'};'}"
    cl2 = ',\n'.join(c2)
    o2 = f"{casetype} cases_y = {'{'}{cl2}{'};'}"
    cl3 = ',\n'.join(c3)
    o3 = f"{casetype} cases_z = {'{'}{cl3}{'};'}"

    contents = '\n'.join([local_header_str, header_str, o1, o2, o3])
    return contents


def generate_hfile() -> str:
    # Guard
    header_gaurd = "__JL_TESTADD_CASES_H__"
    guard_begin = f"#ifndef {header_gaurd}"  + "\n" + f"#define {header_gaurd}"
    guard_end = "#endif"

    # Includes
    include_global = ["vector"]
    include_local = []

    local_header_str = '\n'.join([f"#include\"{lh}\"" for lh in include_local])
    global_header_str = '\n'.join([f"#include<{h}>" for h in include_global])

    # Variables
    header_vars_map = {
            'extern std::vector<std::vector<uint8_t>>': ['cases_x', 'cases_y', 'cases_z']
    }

    header_vars_list = []
    for k, v in header_vars_map.items():
        for name in v:
            header_vars_list.append(f"{k} {name};")
    header_vars = "\n".join(header_vars_list)

    contents = "\n".join([guard_begin,
                               local_header_str, global_header_str, 
                               header_vars,
                               guard_end])
    return contents


if __name__ == '__main__':
    c_file_name = "cases.cpp"
    h_file_name = "cases.h"

    c_file_contents = generate_cfile()
    h_file_contents = generate_hfile()

    with open(c_file_name, 'w') as f:
      f.write(c_file_contents)
    with open(h_file_name, 'w') as f:
      f.write(h_file_contents)
