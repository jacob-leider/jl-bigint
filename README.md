# jl-bigint
A bare-bones c library for manipulating integers of arbitrary size

## Running Tests

I'll add a makefile at some point to make this easier. For now, cd into a subdirectory of "test", run the test-generator python script,

```python3 gen_tests.py```

and compile everything:

```g++ -std=c++11 main.c cases.cpp ../testutils.c ../../src/add_sub_mul.c```
