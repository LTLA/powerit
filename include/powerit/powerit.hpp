#ifndef POWERIT_POWERIT_HPP
#define POWERIT_POWERIT_HPP

#include <algorithm>
#include <numeric>
#include <vector>
#include <cmath>
#include "aarand/aarand.hpp"

/**
 * @file powerit.hpp
 *
 * @brief Implements some power iterations.
 */

namespace powerit {

/**
 * @brief Options for `compute()`.
 */
struct Options {
    /**
     * Maximum number of iterations to perform.
     * Note that the algorithm may converge before reaching this limit.
     */
    int iterations = 500;

    /**
     * Tolerance used to determine convergence.
     * The error is defined as the L2 norm of the difference between eigenvectors at successive iterations;
     * if this drops below `t`, we assume that the algorithm has converged.
     */
    double tolerance = 0.000001;

    /**
     * Number of threads to use for the matrix multiplication.
     *
     * By default, `compute()` uses OpenMP for parallelization.
     * Applications can override this by setting the `POWERIT_CUSTOM_PARALLEL` function-like macro, e.g., in environments where OpenMP is not available.
     * This macro should accept three arguments:
     * - `order`, a `size_t` specifying the number of dimensions in the input matrix.
     * - `num_threads`, an `int` specifying the number of threads to use.
     * - `fun`, a function that accepts two `size_t` values.
     * .
     * The macro should partition the matrix dimension elements into blocks, assign each block to a thread and call `fun()` in each thread with the start position and length of its block.
     */
    int num_threads = 1;
};

/**
 * @cond
 */
template<typename Data_>
Data_ normalize(int ndim, Data_* x) {
    Data_ ss = 0;
    for (int d = 0; d < ndim; ++d) {
        ss += x[d] * x[d];
    }

    if (ss) {
        ss = std::sqrt(ss);
        for (int d = 0; d < ndim; ++d) {
            x[d] /= ss;
        }
    }
    return ss;
}
/**
 * @endcond
 */

template<typename Data_>
struct Result {
    /**
     * Estimate of the first eigenvalue.
     */
    Data_ value;

    /**
     * Number of iterations required to convergence.
     * Set to -1 if convergence did not occur before the maximum number of iterations specified in `Options::iterations`.
     */
    int iterations;
};

/**
 * Perform power iterations on a diagonizable matrix to find the first eigenvalue/vector.
 * This overload generates a starting vector from an existing (P)RNG. 
 *
 * @tparam Data_ Floating-point type for the data.
 * @tparam Engine_ Any C++11-compliant random number generator class.
 *
 * @param order Order of the square matrix.
 * @param[in] matrix Pointer to an array containing an `order`-by-`order` diagonalizable matrix.
 * This is assumed to be row-major.
 * (For symmetric matrices, a column-major layout can also be used.)
 * @param[out] vector Pointer to an array of length `order`.
 * On output, this contains the estimate of the first eigenvector.
 * @param engine Instance of the random number generator.
 * @param opt Further options.
 *
 * @return Result containing the first eigenvalue and other diagnostics.
 */
template<typename Data_, class Engine_>
Result<Data_> compute(size_t order, const Data_* matrix, Data_* vector, Engine_& engine, const Options& opt) {
    // Defining a random starting vector.
    while (1) {
        for (size_t d = 1; d < order; d += 2) {
            auto sampled = aarand::standard_normal<Data_>(engine);
            vector[d - 1] = sampled.first;
            vector[d] = sampled.second;
        }
        if (order % 2) {
            vector[order - 1] = aarand::standard_normal<Data_>(engine).first;
        }
        if (normalize(order, vector)) {
            break;
        }
    }

    return compute(order, matrix, vector, opt);
}

/**
 * Perform power iterations on a diagonizable matrix to find the first eigenvalue/vector.
 * This overload assumes that a random starting vector has already been generated.
 *
 * @tparam Data_ Floating-point type for the data.
 *
 * @param order Order of the square matrix.
 * @param[in] matrix Pointer to an array containing an `order`-by-`order` diagonalizable matrix.
 * This is assumed to be row-major.
 * (For symmetric matrices, a column-major layout can also be used.)
 * @param[in,out] vector Pointer to an array of length `order`.
 * On input, this should contain a random starting vector.
 * On output, this contains the estimate of the first eigenvector.
 * @param opt Further options.
 *
 * @return Result containing the first eigenvalue and other diagnostics.
 */
template<typename Data_>
Result<Data_> compute(size_t order, const Data_* matrix, Data_* vector, const Options& opt) {
    Result<Data_> stats;
    auto& l2 = stats.value;
    stats.iterations = -1;
    std::vector<Data_> buffer(order);

    for (int i = 0; i < opt.iterations; ++i) {
#ifndef POWERIT_CUSTOM_PARALLEL
#ifdef _OPENMP
        #pragma omp parallel for num_threads(opt.num_threads)
#endif
        for (size_t j = 0; j < order; ++j) {
#else
        POWERIT_CUSTOM_PARALLEL(order, opt.num_threads, [&](size_t start, size_t length) -> void {
        for (size_t j = start, end = start + length; j < end; ++j) {
#endif

            // Note that j and order are already both 'size_t', so no need to cast to avoid overflow.
            buffer[j] = std::inner_product(vector, vector + order, matrix + j * order, static_cast<Data_>(0.0));

#ifndef POWERIT_CUSTOM_PARALLEL
        }
#else
        }
        });
#endif

        // Normalizing the vector.
        l2 = normalize(order, buffer.data());

        // Assuming convergence if the vector did not change much from the last iteration.
        Data_ err = 0;
        for (size_t d = 0; d < order; ++d) {
            Data_ diff = buffer[d] - vector[d];
            err += diff * diff;
        }
        if (std::sqrt(err) < opt.tolerance) {
            stats.iterations = i + 1;
            break;
        }

        std::copy(buffer.begin(), buffer.end(), vector);
    }

    return stats;
} 

}

#endif
