# Power iterations in C++

![Unit tests](https://github.com/LTLA/powerit/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/powerit/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/LTLA/powerit/branch/master/graph/badge.svg?token=quUWNz5h2u)](https://codecov.io/gh/LTLA/powerit)

## Overview

Not much to say here, this repository just contains a header-only C++ library to perform power iterations.
It's a quick-and-dirty method of getting the first eigenvector from a diagonalizable matrix - most typically from a covariance matrix, to get the first principal component.
Well, maybe it's not so quick, but it doesn't add any dependencies and it'll get the job done.
And sometimes that's enough.

## Quick start

This is a header-only library, so usage is pretty simple:

```cpp
#include "powerit/PowerIterations.hpp"
#include <random>

// Fill up the input matrix.
size_t ndim = 10;
std::vector<double> matrix(ndim * ndim);

// Compute the power iterations.
powerit::PowerIterations runner;
std::vector<double> eigenvector(ndim);
std::mt19937_64 rng(10);
runner.run(order, matrix.data(), eigenvector.data(), rng);
```

## Building projects 

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```
include(FetchContent)

FetchContent_Declare(
  powerit 
  GIT_REPOSITORY https://github.com/LTLA/powerit
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(powerit)
```

Then you can link to **powerit** to make the headers available during compilation:

```
# For executables:
target_link_libraries(myexe powerit)

# For libaries
target_link_libraries(mylib INTERFACE powerit)
```

If you're not using CMake, the simple approach is to just copy the files - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This requires the additional [**aarand**](https://github.com/LTLA/aarand) library for some lightweight distribution functions.
