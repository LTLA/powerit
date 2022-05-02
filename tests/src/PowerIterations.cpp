#include <gtest/gtest.h>
#include "powerit/PowerIterations.hpp"
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
    powerit::PowerIterations runner;
    std::vector<double> output(order);
    auto res = runner.run(order, x.data(), output.data(), eng);

    // Verifying that it converged.
    EXPECT_TRUE(res.first > 0);
    EXPECT_TRUE(res.second >= 0);

    // Checking that the output is actually an eigenvector.
    double delta = 0;
    for (size_t i = 0; i < order; ++i) {
        auto prod = std::inner_product(output.begin(), output.end(), x.begin() + i * order, 0.0);
        auto diff = prod - res.first * output[i];
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
    powerit::PowerIterations runner;
    runner.set_tolerance(0).set_iterations(100);
    std::vector<double> output(order);
    auto res = runner.run(order, x.data(), output.data(), eng);

    // Checking that it failed to converge.
    EXPECT_EQ(res.second, -1);

    // Checking that the output is still an eigenvector, though.
    EXPECT_TRUE(res.first >= 0);
    double delta = 0;
    for (size_t i = 0; i < order; ++i) {
        auto prod = std::inner_product(output.begin(), output.end(), x.begin() + i * order, 0.0);
        auto diff = prod - res.first * output[i];
        delta += diff * diff;
    }
    EXPECT_TRUE(delta < 0.00000001);
}

