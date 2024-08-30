#ifndef POWERIT_SIMPLE_HPP
#define POWERIT_SIMPLE_HPP

#include "core.hpp"
#include <numeric>

/**
 * @file simple.hpp
 *
 * @brief Power iterations for a simple array.
 */

#ifndef POWERIT_CUSTOM_PARALLEL
#include "subpar/subpar.hpp"
#endif

namespace powerit {

/**
 * @tparam Task_ Integer type for the number of tasks.
 * @tparam Run_ Function to execute a range of tasks.
 *
 * @param num_workers Number of workers.
 * @param num_tasks Number of tasks.
 * @param run_task_range Function to iterate over a range of tasks within a worker.
 *
 * By default, this is an alias to `subpar::parallelize_range()`.
 * However, if the `POWERIT_CUSTOM_PARALLEL` function-like macro is defined, it is called instead. 
 * Any user-defined macro should accept the same arguments as `subpar::parallelize_range()`.
 */
template<typename Task_, class Run_>
void parallelize(int num_workers, Task_ num_tasks, Run_ run_task_range) {
#ifndef POWERIT_CUSTOM_PARALLEL
    // We can set nothrow_ = true because of the simplicity of the code below;
    // no explicit throws, no allocations that could throw bad_alloc, just math.
    subpar::parallelize_range<true>(num_workers, num_tasks, std::move(run_task_range));
#else
    POWERIT_CUSTOM_PARALLEL(num_workers, num_tasks, run_task_range);
#endif
}

/**
 * Perform power iterations on a diagonizable matrix to find the first eigenvalue/vector.
 * This overload generates a starting vector from an existing (P)RNG. 
 *
 * @tparam Data_ Floating-point type for the data.
 * @tparam Engine_ Any C++11-compliant random number generator class.
 *
 * @param order Order of the square matrix.
 * @param[in] matrix Pointer to an array containing an `order`-by-`order` diagonalizable matrix.
 * @param row_major Whether `matrix` is row-major.
 * @param[out] vector Pointer to an array of length `order`.
 * On output, this contains the estimate of the first eigenvector.
 * @param engine Instance of the random number generator.
 * @param opt Further options.
 *
 * @return Result containing the first eigenvalue and other diagnostics.
 */
template<typename Data_, class Engine_>
Result<Data_> compute(size_t order, const Data_* matrix, bool row_major, Data_* vector, Engine_& engine, const Options& opt) {
    fill_starting_vector(order, vector, engine);
    return compute(order, matrix, row_major, vector, opt);
}

/**
 * Perform power iterations on an array containing a diagonizable matrix. 
 * This overload assumes that a random starting vector has already been generated.
 *
 * @tparam Data_ Floating-point type for the data.
 *
 * @param order Order of the square matrix.
 * @param[in] matrix Pointer to an array containing an `order`-by-`order` diagonalizable matrix.
 * @param row_major Whether `matrix` is row-major.
 * @param[in,out] vector Pointer to an array of length `order`.
 * On input, this should contain a random starting vector.
 * On output, this contains the estimate of the first eigenvector.
 * @param opt Further options.
 *
 * @return Result containing the first eigenvalue and other diagnostics.
 */
template<typename Data_>
Result<Data_> compute(size_t order, const Data_* matrix, bool row_major, Data_* vector, const Options& opt) {
    if (row_major) {
        return compute_core(order, [&](std::vector<Data_>& buffer, const Data_* vec) {
            parallelize(opt.num_threads, order, [&](int, size_t start, size_t length) {
                for (size_t j = start, end = start + length; j < end; ++j) {
                    // Note that j and order are already both 'size_t', so no need to cast to avoid overflow.
                    buffer[j] = std::inner_product(vec, vec + order, matrix + j * order, static_cast<Data_>(0.0));
                }
            });
        }, vector, opt);

    } else if (opt.num_threads == 1) { 
        // Dedicated path to avoid allocating a per-thread temporary.
        return compute_core(order, [&](std::vector<Data_>& buffer, const Data_* vec) {
            std::fill(buffer.begin(), buffer.end(), 0);
            auto matcopy = matrix;
            for (size_t j = 0; j < order; ++j) {
                Data_ mult = vec[j];
                for (size_t k = 0; k < order; ++k, ++matcopy) {
                    buffer[k] += mult * (*matcopy);
                }
            }
        }, vector, opt);

    } else {
        // We make a separate buffer for each thread to avoid false sharing problems.
        // We do the allocation outside so that (i) we can re-use memory, and
        // (ii) the code inside the parallelize() cannot throw.
        std::vector<std::vector<Data_> > temp_buffers(opt.num_threads);
        for (int i = 0; i < opt.num_threads; ++i) {
            temp_buffers[i].resize(order);
        }

        return compute_core(order, [&](std::vector<Data_>& buffer, const Data_* vec) {
            parallelize(opt.num_threads, order, [&](int t, size_t start, size_t length) {
                auto& tmp = temp_buffers[t];
                std::fill_n(tmp.begin(), length, 0);

                size_t offset = start; // already size_t's, no need to cast.
                for (size_t j = 0; j < order; ++j, offset += order) {
                    auto mult = vec[j];
                    auto matcopy = matrix + offset;
                    for (size_t k = 0; k < length; ++k, ++matcopy) {
                        tmp[k] += mult * (*matcopy);
                    }
                }

                std::copy_n(tmp.begin(), length, buffer.begin() + start);
            });
        }, vector, opt);
    } 
}

}

#endif
