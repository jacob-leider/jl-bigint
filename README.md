# jl-bigint
A bare-bones c library for manipulating integers of arbitrary size. My test framework uses c++ (and a bit of python scripting), but everything in `src` is pure c. This exists solely for the purpose of allowing myself to mess around with the internals of a big-integer library, and tailor the behavior for my specific use cases.

Do not use this library. Use [GMP](https://gmplib.org).

## Documentation

Documentation can be obtained by running [doxygen](https://www.doxygen.nl/download.html) with source code root `jl-bigint/src`, or just reading the source code.

## Unit Testing

Run `make` to generate the testcases and compile the unit testing program. This will:

1. Run the `test/<test-dir>/python3 gen_tests.py` script, which creates the files `cases.cpp` and `cases.h`. These contain three vectors of equal length, where entries of the same index are a triplet `(x, y, z)` satisfying some binary operation (e.g. `x` + `y` = `z`). These values are partially randomized and partially hand-selected to test edge cases.

2. Creates a program `main` which tests the corresponding function in `src/<file-containing-the-function-being-tested>` against all cases, and reports the results.
