#include <gtest/gtest.h>

#ifdef TEST_CUSTOM_PARALLEL
#include <thread>
template<class Function_>
void parallelize(size_t order, int nthreads, Function_ fun) {
    size_t per_job = (order / nthreads) + (order % nthreads > 0);

    std::vector<std::thread> jobs;
    jobs.reserve(nthreads);

    size_t start = 0;
    for (size_t o = 0; o < order; ++o) {
        size_t length = std::min(order - start, per_job);
        jobs.emplace_back(fun, start, length);
        start += length;
    }

    for (auto& j : jobs) {
        j.join();
    }
}

#define POWERIT_CUSTOM_PARALLEL parallelize
#endif

#include "powerit/powerit.hpp"
#include "aarand/aarand.hpp"
#include <random>
#include <vector>

template<class Engine>
std::vector<double> mock_square_matrix(size_t order, Engine& eng) {
    std::vector<double> x(order * order);
    for (size_t i = 0; i < order; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            auto val = aarand::standard_uniform<double>(eng);
            x[i * order + j] = val;
            x[j * order + i] = val;
        }
    }
    return x;
}

TEST(PowerIterations, Basic) {
    // Mocking up a symmetric matrix.
    size_t order = 5;
    std::mt19937_64 eng(123);
    auto x = mock_square_matrix(order, eng);

    // Running power iterations.
    std::vector<double> output(order);
    auto res = powerit::compute(order, x.data(), output.data(), eng, powerit::Options());

    // Verifying that it converged.
    EXPECT_TRUE(res.value > 0);
    EXPECT_TRUE(res.iterations >= 0);

    // Checking that the output is actually an eigenvector.
    double delta = 0;
    for (size_t i = 0; i < order; ++i) {
        auto prod = std::inner_product(output.begin(), output.end(), x.begin() + i * order, 0.0);
        auto diff = prod - res.value * output[i];
        delta += diff * diff;
    }
    EXPECT_TRUE(delta < 0.00000001);
}

TEST(PowerIterations, Unconverged) {
    // Mocking up a symmetric matrix.
    size_t order = 5;
    std::mt19937_64 eng(123);
    auto x = mock_square_matrix(order, eng);

    // Running power iterations.
    powerit::Options opt;
    opt.tolerance = 0;
    opt.iterations = 100;
    std::vector<double> output(order);
    auto res = powerit::compute(order, x.data(), output.data(), eng, opt);

    // Checking that it failed to converge.
    EXPECT_EQ(res.iterations, -1);

    // Checking that the output is still an eigenvector, though.
    EXPECT_TRUE(res.value >= 0);
    double delta = 0;
    for (size_t i = 0; i < order; ++i) {
        auto prod = std::inner_product(output.begin(), output.end(), x.begin() + i * order, 0.0);
        auto diff = prod - res.value * output[i];
        delta += diff * diff;
    }
    EXPECT_TRUE(delta < 0.00000001);
}

TEST(PowerIterations, Parallel) {
    size_t order = 14;
    std::mt19937_64 eng(123);
    auto x = mock_square_matrix(order, eng);

    std::vector<double> init(order);
    for (size_t d = 0; d < order; ++d) {
        init[d] = aarand::standard_uniform<double>(eng);
    }

    powerit::Options opt;
    std::vector<double> ref(init);
    auto res = powerit::compute(order, x.data(), ref.data(), opt);

    // Adding more threads.
    opt.num_threads = 3;
    std::vector<double> par(init);
    auto pres = powerit::compute(order, x.data(), par.data(), opt);

    EXPECT_EQ(par, ref);
    EXPECT_EQ(res.value, pres.value);
    EXPECT_EQ(res.iterations, pres.iterations);
}
