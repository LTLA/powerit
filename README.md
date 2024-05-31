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
#include "powerit/powerit.hpp"
#include <random>

// Fill up the input matrix (row-major).
size_t ndim = 10;
std::vector<double> matrix(ndim * ndim);

// Compute the power iterations.
std::vector<double> eigenvector(ndim);
powerit::Options opt;
std::mt19937_64 rng(10);
auto info = powerit::compute(order, matrix.data(), eigenvector.data(), rng, opt);

info.value; // estimate of the first eigenvalue
info.iterations; // number of iterations required for convergence.
```

Users can tune the number of iterations, tolerance, and number of threads via the `Options` argument.
Check out the [API reference](https://ltla.github.io/powerit) for more information.

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  powerit 
  GIT_REPOSITORY https://github.com/LTLA/powerit
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(powerit)
```

Then you can link to **powerit** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe ltla::powerit)

# For libaries
target_link_libraries(mylib INTERFACE ltla::powerit)
```

### CMake with `find_package()`

To install the library, clone the desired version of this repository and run:

```sh
mkdir build && cd build
cmake .. -DPOWERIT_TESTS=OFF
cmake --build . --target install
```

Then, we can use `find_package()` as usual:

```cmake
find_package(ltla_powerit CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE ltla::powerit)
```

By default, this will use `FetchContent` to fetch all external dependencies (listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt)).
If you want to install them manually, use `-DPOWERIT_FETCH_EXTERN=OFF`.

### Manual

If you're not using CMake, the simple approach is to just copy the files in `include/` - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This requires the external dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt), which also need to be made available during compilation.
