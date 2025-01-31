# jl-bigint
A bare-bones c library for manipulating integers of arbitrary size

## Running Tests

For now, cd into a subdirectory of "test", run the test-generator python script,

```python3 gen_tests.py```

and compile everything:

```g++ -std=c++11 main.c cases.cpp ../testutils.c ../../src/add_sub_mul.c```
