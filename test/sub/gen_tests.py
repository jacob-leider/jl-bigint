#!/usr/bin/env python3

# Run this in its directory to generate test cases.

import random
import os
import math


def big_int_to_c_array(*args):
    args_out = []
    for x in args:
        x_hex = f"{x:X}"
        if len(x_hex) % 2 == 1:
            x_hex = '0' + x_hex
        x_hex_bytes = [x_hex[i:i + 2] for i in range(0,len(x_hex),2)]
        x_str = f"{'{'}0x{', 0x'.join(x_hex_bytes)}{'}'}"
        args_out.append(x_str)
    return args_out


# z_size must be a multiple of 8.
def case_str(x: int, y: int, z_size=None) -> tuple[str, str, str]:
    z = x - y 

    # Default z size.
    if z_size == None:
        z_size = math.ceil(math.log2(abs(z)))
        z_size = (z_size // 8) * 8

    # When y > x:
    #       z = 2^z_size - ((y - x) % 2^z_size)
    # z fits in z_size bits.
    if (z < 0):
        z = z % 2**(z_size)

    x_str, y_str, z_str = big_int_to_c_array(x, y, z)
    return x_str, y_str, z_str


def generate_cfile() -> str:
    c1 = []
    c2 = []
    c3 = []

    # Random cases: y < x.
    for i in range(10):
        x = random.randint(2**100, 2**101)
        y = random.randint(2**100, 2**101)

        if x < y:
            x, y, = y, x
        
        t1, t2, t3 = case_str(x, y)

        c1.append(t1)
        c2.append(t2)
        c3.append(t3)

  # Random cases: y > x.
    for i in range(10):
        x_size = 100
        y_size = 90
        # Try different z sizes. Tests output buffer being too large and too 
        # small.
        for z_size in range(8, 150, 16):
            x = random.randint(2**x_size, 2**(x_size + 1))
            y = random.randint(2**y_size, 2**(y_size + 1))
            
            t1, t2, t3 = case_str(x, y, z_size=z_size)

            c1.append(t1)
            c2.append(t2)
            c3.append(t3)
    
    # Edge cases.
    for i in range(40,60):
        x = 2**(i + 1) 
        y = 2**i
        for z_size in range(8, 150, 16):
            t1, t2, t3 = case_str(x, y, z_size=z_size)
            c1.append(t1)
            c2.append(t2)
            c3.append(t3)

            t1, t2, t3 = case_str(y, x, z_size=z_size)
            c1.append(t1)
            c2.append(t2)
            c3.append(t3)

        x = 2**(i + 1) - 1
        y = 2**i - 1
        for z_size in range(1, 2 * i, 10):
            t1, t2, t3 = case_str(x, y, z_size=z_size)

            c1.append(t1)
            c2.append(t2)
            c3.append(t3)




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
