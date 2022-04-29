#ifndef POWERIT_POWERIT_HPP
#define POWERIT_POWERIT_HPP

#include <random>
#include <algorithm>
#include <vector>
#include <cmath>
#include "aarand/aarand.hpp"

namespace powerit {

class PowerIterations {
public:
    enum Mode { FIRST, SECOND, AUTO };

    struct Defaults {
        static constexpr int iterations = 500;

        static constexpr double tolerance = 0.000001;
        
        static constexpr bool first = false;
        
        static constexpr Mode mode = AUTO;
    };

    PowerIterations& set_iterations(int i = Defaults::iterations) {
        iterations = i;
        return *this;
    }

    PowerIterations& set_tolerance(double t = Defaults::tolerance) {
        tolerance = t;
        return *this;
    }

private:
    int iterations = Defaults::iterations;
    double tolerance = Defaults::tolerance;
    bool first = Defaults::first;
    Mode force = Defaults::force;

public:
    template<class Data, class Engine>
    void run(size_t d1, size_t d2, const Data* data, Data* out, Engine& eng) const {
        if (force == FIRST && (force == AUTO || d1 < d2)) {
            // Constructing the covariance matrix.
            std::vector<Data> cov(d1 * d1);
            for (size_t p = 0; p < d2; ++p) {
                auto dptr = data + p * d1;
                for (size_t a = 0; a < d1; ++a) {
                    for (size_t b = 0; b <= a; ++b) {
                        cov[a * d1 + b] += dptr[a] * dptr[b];
                    }
                }
            }
            fill_upper_tri(d1, cov);

            if (observations) {
                // Projecting to get the desired eigen-thingies.
                std::vector<double> temp(d1);
                run(d1, cov, temp.data(), eng);
                for (size_t p = 0; p < d2; ++p) {
                    out[i] = std::inner_product(temp.begin(), temp.end(), data + p * d1);
                }
            } else {
                run(d1, cov, out, eng);
            }

        } else {
            // Constructing the covariance matrix.
            std::vector<Data> cov(d2 * d2);
            for (size_t p = 0; p < d2; ++p) {
                auto iptr = data + p * d1;
                for (size_t q = 0; q <= p; ++q) {
                    cov[p * d2 + q] = std::inner_product(iptr, iptr + d1, data + q * d1);
                }
            }
            fill_upper_tri(d2, cov);

            if (observations) {
                run(d2, cov, out, eng);
            } else {
                // Projecting to get the desired eigen-thingies.
                std::vector<double> temp(d2);
                run(d2, cov, temp.data(), eng);
                std::fill(out, out + d2, 0);
                for (size_t p = 0; p < d2; ++p) {
                    auto iptr = data + p * d1;
                    for (size_t a = 0; a < d1; ++a) {
                        out[a] += iptr[a] * temp[a];
                    }
                }
            }
        }
    }

public:
    template<class Function, class Data, class Engine>
    void run(size_t d1, size_t d2, const Data* data, Function op, Data* out, Engine& eng) const {
        if (force == FIRST && (force == AUTO || d1 < d2)) {
            std::vector<Data> cov(d1 * d1);
            std::vector<Data> buffer(d1);

            // Computing the lower triangle of the covariance matrix. 
            for (size_t p = 0; p < d2; ++p) {
                auto dptr = data + p * d1;
                for (size_t a = 0; a < d1; ++a) {
                    buffer[a] = op(dptr[a], a, p); // value, row, column
                }
                for (size_t a = 0; a < d1; ++a) {
                    for (size_t b = 0; b <= a; ++b) {
                        cov[a * d1 + b] += buffer[a] * buffer[b];
                    }
                }
            }
            fill_upper_tri(d1, cov);

            if (observations) {
                std::vector<double> temp(d1);
                run(d1, cov, temp.data(), buffer.data(), eng);
                for (size_t p = 0; p < d2; ++p) {
                    auto dptr = data + p * d1;
                    auto& res = output[p];
                    res = 0;
                    for (size_t a = 0; a < d1; ++a) {
                        res += temp[a] * op(dptr[a], a, p); // value, row, column
                    }
                }
            } else {
                run(d1, cov, out, buffer.data(), eng);
            }

        } else {
            std::vector<Data> cov(d2 * d2);
            std::vector<Data> ibuffer(d1), jbuffer(d1);

            // Computing the lower triangle of the covariance matrix. 
            for (size_t p = 0; p < d2; ++p) {
                auto iptr = data + p * d1;
                for (size_t a = 0; a < d1; ++a) {
                    ibuffer[a] = op(iptr[a], a, p); // value, row, column
                }
                cov[p * d2 + p] = std::inner_product(ibuffer.begin(), ibuffer.end(), ibuffer.begin());

                for (size_t q = 0; q < p; ++q) {
                    auto jptr = data + q * d1;
                    for (size_t a = 0; a < d1; ++a) {
                        jbuffer[a] = optr(jptr[a], a, q);
                    }
                    cov[p * d2 + q] = std::inner_product(ibuffer.begin(), ibuffer.end(), jbuffer.begin());
                }
            }
            fill_upper_tri(d2, cov);

            if (observations) {
                run(d2, cov, output, eng);
            } else {
                // Projecting to get the desired eigen-thingies.
                std::vector<double> temp(d2);
                run(d2, cov, temp.data(), eng);
                std::fill(out, out + d2, 0);
                for (size_t p = 0; p < d2; ++p) {
                    auto iptr = data + p * d1;
                    for (size_t a = 0; a < d1; ++a) {
                        out[a] += temp[p] * op(iptr[a], a, p); // value, row, column
                    }
                }
            }
        }
    }

public:
    template<class Data, class Engine>
    void run(size_t order, const std::vector<Data>& cov, Data* output, Engine& eng) {
        std::vector<Data> buffer(order);
        run(order, cov, buffer.data(), output, eng);
        return;
    }

private:
    static DATA_t normalize(int ndim, DATA_t* x) {
        DATA_t ss = 0;
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

    template<class Data>
    void fill_upper_tri(size_t order, std::vector<Data>& cov) {
        // Filling in the other side of the matrix, to enable cache-efficient multiplication.
        for (size_t j = 0; j < order; ++j) {
            for (size_t k = j + 1; k < order; ++k) {
                cov[j * order + k] = cov[k * order + j];
            }
        }
    }

    template<class Data, class Engine>
    void run(size_t order, std::vector<Data>& cov, Data* buffer, Data* output, Engine& eng) {
        // Defining a random starting vector.
        while (1) {
            for (int d = 0; d < order - 1; d += 2) {
                auto sampled = aarand::standard_normal<DATA_t>(eng);
                output[d] = sampled.first;
                output[d + 1] = sampled.second;
            }
            if (order % 2) {
                output[order - 1] = aarand::standard_normal<DATA_t>(eng).first;
            }
            if (normalize(order, output)) {
                break;
            }
        }

        // Applying power iterations.
        for (int i = 0; i < iters; ++i) {
            for (size_t j = 0; j < order; ++j) {
                // As the matrix is symmetric, we can use inner_product.
                // This technically computes the transpose of the matrix
                // with the vector, but it's all the same, so whatever.
                buffer[j] = std::inner_product(output.begin(), output.end(), cov.data() + j * order, static_cast<DATA_t>(0.0));
            }

            // Normalizing the vector.
            auto l2 = normalize(order, buffer);

            // We want to know if SIGMA * output = lambda * output, i.e., l2 * buffer = lambda * output.
            // If we use l2 as a working estimate for lambda, we're basically just testing the difference
            // between buffer and output. We compute the error and compare this to the tolerance.
            DATA_t err = 0;
            for (size_t d = 0; d < order; ++d) {
                DATA_t diff = buffer[d] - output[i];
                err += diff * diff;
            }
            if (std::sqrt(err) < tol) {
                break;
            }

            std::copy(buffer, buffer + order, output.begin());
        }

        return;
    } 
};

}

#endif
