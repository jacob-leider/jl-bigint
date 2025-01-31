import random
import os

if __name__ == '__main__':
  c_file_name = "cases.cpp"
  h_file_name = "cases.h"
  
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
  
  file_contents = '\n'.join([local_header_str, header_str, o1, o2, o3])
  
  with open(c_file_name, 'w') as f:
    f.write(file_contents)
