#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstddef>
#include <cassert>

namespace DynamicMatrix {

    // A dynamic matrix type: rows and columns determined at runtime
    template<typename T>
    using Mat = std::vector<std::vector<T>>;

    // Create a matrix of size rows x cols, filled with 'init' (default-constructed T)
    template<typename T>
    Mat<T> create(std::size_t rows, std::size_t cols, T init = T{}) {
        if (rows == 0 || cols == 0)
            throw std::invalid_argument(
                    "DynamicMatrix::create: 'rows' and 'cols' must be > 0"
            );
        return Mat<T>(rows, std::vector<T>(cols, init));
    }

    // Generate an identity matrix of size n x n
    template<typename T>
    Mat<T> identity(std::size_t n) {
        if (n == 0)
            throw std::invalid_argument(
                    "DynamicMatrix::identity: size must be > 0"
            );
        Mat<T> I = create<T>(n, n, T{});
        for (std::size_t i = 0; i < n; ++i) {
            I[i][i] = T(1);
        }
        return I;
    }

    // Helper: verify two matrices have the same dimensions
    inline void check_same_size(
            std::size_t r1, std::size_t c1,
            std::size_t r2, std::size_t c2,
            const char *op_name
    ) {
        if (r1 != r2 || c1 != c2) {
            throw std::invalid_argument(
                    std::string("DynamicMatrix::") + op_name + ": dimension mismatch (" +
                    std::to_string(r1) + "x" + std::to_string(c1) + " vs " +
                    std::to_string(r2) + "x" + std::to_string(c2) + ")"
            );
        }
    }

    template<typename T>
    T mod_reduce(T x, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::mod_reduce: mod must be > 0");
        x %= mod;
        if (x < T(0)) x += mod;
        return x;
    }

    // Add two matrices of equal size
    template<typename T>
    Mat<T> add(const Mat<T> &A, const Mat<T> &B) {
        std::size_t R = A.size();
        std::size_t C = (R > 0 ? A[0].size() : 0);
        check_same_size(R, C, B.size(), (B.size() > 0 ? B[0].size() : 0), "add");
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = A[i][j] + B[i][j];
        return result;
    }

    // Modular matrix addition
    template<typename T>
    Mat<T> add(const Mat<T> &A, const Mat<T> &B, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::add mod: mod must be > 0");
        std::size_t R = A.size();
        std::size_t C = (R > 0 ? A[0].size() : 0);
        check_same_size(R, C, B.size(), (B.size() > 0 ? B[0].size() : 0), "add(mod)");
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = mod_reduce(A[i][j] + B[i][j], mod);
        return result;
    }

    // Subtract matrix B from A (same size check)
    template<typename T>
    Mat<T> sub(const Mat<T> &A, const Mat<T> &B) {
        std::size_t R = A.size();
        std::size_t C = (R > 0 ? A[0].size() : 0);
        check_same_size(R, C, B.size(), (B.size() > 0 ? B[0].size() : 0), "sub");
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = A[i][j] - B[i][j];
        return result;
    }

    // Modular matrix subtraction
    template<typename T>
    Mat<T> sub(const Mat<T> &A, const Mat<T> &B, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::sub mod: mod must be > 0");
        std::size_t R = A.size();
        std::size_t C = (R > 0 ? A[0].size() : 0);
        check_same_size(R, C, B.size(), (B.size() > 0 ? B[0].size() : 0), "sub(mod)");
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = mod_reduce(A[i][j] - B[i][j], mod);
        return result;
    }

    // Multiply every element by scalar 's'
    template<typename T>
    Mat<T> mul(const Mat<T> &A, T s) {
        std::size_t R = A.size();
        if (R == 0)
            throw std::invalid_argument(
                    "DynamicMatrix::mul: matrix must not be empty"
            );
        std::size_t C = A[0].size();
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = A[i][j] * s;
        return result;
    }

    // Modular scalar multiplication
    template<typename T>
    Mat<T> mul(const Mat<T> &A, T s, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::mul mod: mod must be > 0");
        std::size_t R = A.size();
        if (R == 0)
            throw std::invalid_argument("DynamicMatrix::mul mod: matrix must not be empty");
        std::size_t C = A[0].size();
        Mat<T> result = create<T>(R, C);
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                result[i][j] = mod_reduce(A[i][j] * s, mod);
        return result;
    }

    // Multiply matrix A (R×K) by matrix B (K×C) → result (R×C)
    template<typename T>
    Mat<T> mul(const Mat<T> &A, const Mat<T> &B) {
        std::size_t R = A.size();
        if (R == 0)
            throw std::invalid_argument(
                    "DynamicMatrix::mul: left matrix has zero rows"
            );
        std::size_t K = A[0].size();
        if (B.size() != K)
            throw std::invalid_argument(
                    "DynamicMatrix::mul: number of columns of A must equal number of rows of B"
            );
        std::size_t C = (B.size() > 0 ? B[0].size() : 0);
        Mat<T> result = create<T>(R, C, T{});
        for (std::size_t i = 0; i < R; ++i) {
            if (A[i].size() != K)
                throw std::invalid_argument(
                        "DynamicMatrix::mul: A must be a rectangular matrix"
                );
            for (std::size_t k = 0; k < K; ++k) {
                T tmp = A[i][k];
                for (std::size_t j = 0; j < C; ++j) {
                    if (B[k].size() != C)
                        throw std::invalid_argument(
                                "DynamicMatrix::mul: B must be a rectangular matrix"
                        );
                    result[i][j] += tmp * B[k][j];
                }
            }
        }
        return result;
    }

    // Modular matrix multiplication
    template<typename T>
    Mat<T> mul(const Mat<T> &A, const Mat<T> &B, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::mul mod: mod must be > 0");
        std::size_t R = A.size();
        if (R == 0) throw std::invalid_argument("DynamicMatrix::mul mod: A has zero rows");
        std::size_t K = A[0].size();
        if (B.size() != K)
            throw std::invalid_argument("DynamicMatrix::mul mod: A.columns must equal B.rows");
        std::size_t C = (B.size() > 0 ? B[0].size() : 0);
        Mat<T> result = create<T>(R, C, T{});
        for (std::size_t i = 0; i < R; ++i) {
            if (A[i].size() != K)
                throw std::invalid_argument("DynamicMatrix::mul mod: A must be rectangular");
            for (std::size_t k = 0; k < K; ++k) {
                T tmp = mod_reduce(A[i][k], mod);
                for (std::size_t j = 0; j < C; ++j) {
                    if (B[k].size() != C)
                        throw std::invalid_argument("DynamicMatrix::mul mod: B must be rectangular");
                    result[i][j] = mod_reduce(result[i][j] + tmp * B[k][j], mod);
                }
            }
        }
        return result;
    }

    // Raise a square matrix to the power 'e' using binary exponentiation
    template<typename T>
    Mat<T> pow(Mat<T> base, std::uint64_t e) {
        std::size_t N = base.size();
        if (N == 0 || base[0].size() != N)
            throw std::invalid_argument(
                    "DynamicMatrix::pow: matrix must be non-empty and square"
            );
        Mat<T> result = identity<T>(N);
        while (e > 0) {
            if (e & 1) result = mul(result, base);
            base = mul(base, base);
            e >>= 1;
        }
        return result;
    }

    // Modular exponentiation for square matrices
    template<typename T>
    Mat<T> pow(Mat<T> base, std::uint64_t e, T mod) {
        if (mod <= T(0))
            throw std::invalid_argument("DynamicMatrix::pow mod: mod must be > 0");
        std::size_t N = base.size();
        if (N == 0 || base[0].size() != N)
            throw std::invalid_argument("DynamicMatrix::pow mod: matrix must be non-empty and square");
        // reduce base mod
        for (std::size_t i = 0; i < N; ++i)
            for (std::size_t j = 0; j < N; ++j)
                base[i][j] = mod_reduce(base[i][j], mod);
        Mat<T> result = identity<T>(N);
        while (e > 0) {
            if (e & 1) result = mul(result, base, mod);
            base = mul(base, base, mod);
            e >>= 1;
        }
        return result;
    }

    // Print matrix to stdout, with dimension header
    template<typename T>
    void debug_print(const Mat<T> &A) {
        std::size_t R = A.size();
        std::size_t C = (R > 0 ? A[0].size() : 0);
        std::cout << "DynamicMatrix<" << R << "x" << C << ">\n";
        for (std::size_t i = 0; i < R; ++i) {
            if (A[i].size() != C)
                throw std::invalid_argument(
                        "DynamicMatrix::debug_print: matrix rows have inconsistent lengths"
                );
            for (std::size_t j = 0; j < C; ++j) {
                std::cout << A[i][j] << ' ';
            }
            std::cout << '\n';
        }
    }

} // namespace DynamicMatrix

int main() {

    std::cout << "Starting DynamicMatrix unit tests...\n";

    // Test for create function
    std::cout << "Testing create function...\n";
    try {
        DynamicMatrix::Mat<int> m1 = DynamicMatrix::create<int>(3, 4, 5);
        assert(m1.size() == 3);
        assert(m1[0].size() == 4);
        assert(m1[1][2] == 5);
        std::cout << "create function test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "create function test failed: " << e.what() << "\n";
    }

    // Test create with invalid dimensions
    std::cout << "Testing create with invalid dimensions...\n";
    try {
        DynamicMatrix::Mat<int> m_invalid = DynamicMatrix::create<int>(0, 5);
        std::cout << "create with invalid dimensions test failed!\n";
    } catch (const std::invalid_argument &) {
        std::cout << "create with invalid dimensions test passed!\n";
    }

    // Test identity function
    std::cout << "Testing identity function...\n";
    try {
        DynamicMatrix::Mat<int> id = DynamicMatrix::identity<int>(3);
        assert(id.size() == 3);
        assert(id[0].size() == 3);
        assert(id[0][0] == 1 && id[0][1] == 0 && id[0][2] == 0);
        assert(id[1][0] == 0 && id[1][1] == 1 && id[1][2] == 0);
        assert(id[2][0] == 0 && id[2][1] == 0 && id[2][2] == 1);
        std::cout << "identity function test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "identity function test failed: " << e.what() << "\n";
    }

    // Test matrix addition
    std::cout << "Testing matrix addition...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 2, 1);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(2, 2, 2);
        DynamicMatrix::Mat<int> sum = DynamicMatrix::add(a, b);
        assert(sum[0][0] == 3 && sum[0][1] == 3);
        assert(sum[1][0] == 3 && sum[1][1] == 3);
        std::cout << "matrix addition test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "matrix addition test failed: " << e.what() << "\n";
    }

    // Test modular matrix addition
    std::cout << "Testing modular matrix addition...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 2, 3);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(2, 2, 4);
        DynamicMatrix::Mat<int> sum_mod = DynamicMatrix::add(a, b, 5);
        assert(sum_mod[0][0] == 2 && sum_mod[0][1] == 2);
        assert(sum_mod[1][0] == 2 && sum_mod[1][1] == 2);
        std::cout << "modular matrix addition test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "modular matrix addition test failed: " << e.what() << "\n";
    }

    // Test matrix subtraction
    std::cout << "Testing matrix subtraction...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 2, 5);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(2, 2, 2);
        DynamicMatrix::Mat<int> diff = DynamicMatrix::sub(a, b);
        assert(diff[0][0] == 3 && diff[0][1] == 3);
        assert(diff[1][0] == 3 && diff[1][1] == 3);
        std::cout << "matrix subtraction test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "matrix subtraction test failed: " << e.what() << "\n";
    }

    // Test modular matrix subtraction
    std::cout << "Testing modular matrix subtraction...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 2, 2);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(2, 2, 4);
        DynamicMatrix::Mat<int> diff_mod = DynamicMatrix::sub(a, b, 5);
        assert(diff_mod[0][0] == 3 && diff_mod[0][1] == 3);
        assert(diff_mod[1][0] == 3 && diff_mod[1][1] == 3);
        std::cout << "modular matrix subtraction test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "modular matrix subtraction test failed: " << e.what() << "\n";
    }

    // Test scalar multiplication
    std::cout << "Testing scalar multiplication...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 3, 2);
        DynamicMatrix::Mat<int> scaled = DynamicMatrix::mul(m, 3);
        assert(scaled[0][0] == 6 && scaled[0][1] == 6 && scaled[0][2] == 6);
        assert(scaled[1][0] == 6 && scaled[1][1] == 6 && scaled[1][2] == 6);
        std::cout << "scalar multiplication test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "scalar multiplication test failed: " << e.what() << "\n";
    }

    // Test modular scalar multiplication
    std::cout << "Testing modular scalar multiplication...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 2, 4);
        DynamicMatrix::Mat<int> scaled_mod = DynamicMatrix::mul(m, 3, 5);
        assert(scaled_mod[0][0] == 2 && scaled_mod[0][1] == 2);
        assert(scaled_mod[1][0] == 2 && scaled_mod[1][1] == 2);
        std::cout << "modular scalar multiplication test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "modular scalar multiplication test failed: " << e.what() << "\n";
    }

    // Test matrix multiplication
    std::cout << "Testing matrix multiplication...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 3, 1);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(3, 2, 2);
        DynamicMatrix::Mat<int> product = DynamicMatrix::mul(a, b);
        assert(product.size() == 2);
        assert(product[0].size() == 2);
        assert(product[0][0] == 6 && product[0][1] == 6);
        assert(product[1][0] == 6 && product[1][1] == 6);
        std::cout << "matrix multiplication test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "matrix multiplication test failed: " << e.what() << "\n";
    }

    // Test modular matrix multiplication
    std::cout << "Testing modular matrix multiplication...\n";
    try {
        DynamicMatrix::Mat<int> a = DynamicMatrix::create<int>(2, 2, 3);
        DynamicMatrix::Mat<int> b = DynamicMatrix::create<int>(2, 2, 4);
        DynamicMatrix::Mat<int> product_mod = DynamicMatrix::mul(a, b, 5);
        assert(product_mod[0][0] == 4 && product_mod[0][1] == 4);
        assert(product_mod[1][0] == 4 && product_mod[1][1] == 4);
        std::cout << "modular matrix multiplication test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "modular matrix multiplication test failed: " << e.what() << "\n";
    }

    // Test matrix exponentiation
    std::cout << "Testing matrix exponentiation...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 2, 2);
        DynamicMatrix::Mat<int> m_squared = DynamicMatrix::pow(m, 2);
        assert(m_squared[0][0] == 8 && m_squared[0][1] == 8);
        assert(m_squared[1][0] == 8 && m_squared[1][1] == 8);
        std::cout << "matrix exponentiation test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "matrix exponentiation test failed: " << e.what() << "\n";
    }

    // Test modular matrix exponentiation
    std::cout << "Testing modular matrix exponentiation...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 2, 2);
        DynamicMatrix::Mat<int> m_pow_mod = DynamicMatrix::pow(m, 3, 10);
        assert(m_pow_mod[0][0] == 2 && m_pow_mod[0][1] == 2);
        assert(m_pow_mod[1][0] == 2 && m_pow_mod[1][1] == 2);
        std::cout << "modular matrix exponentiation test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "modular matrix exponentiation test failed: " << e.what() << "\n";
    }

    // Test debug_print (visual verification)
    std::cout << "Testing debug_print...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 2, 1);
        std::cout << "Expected output:\n";
        std::cout << "DynamicMatrix<2x2>\n1 1\n1 1\n";
        std::cout << "Actual output:\n";
        DynamicMatrix::debug_print(m);
        std::cout << "debug_print test completed (visual verification needed)\n";
    } catch (const std::exception &e) {
        std::cout << "debug_print test failed: " << e.what() << "\n";
    }

    // Edge case: matrix exponentiation with e=0...
    std::cout << "Testing matrix exponentiation with e=0...\n";
    try {
        DynamicMatrix::Mat<int> m = DynamicMatrix::create<int>(2, 2, 5);
        DynamicMatrix::Mat<int> m_pow0 = DynamicMatrix::pow(m, 0);
        assert(m_pow0[0][0] == 1 && m_pow0[0][1] == 0);
        assert(m_pow0[1][0] == 0 && m_pow0[1][1] == 1);
        std::cout << "matrix exponentiation with e=0 test passed!\n";
    } catch (const std::exception &e) {
        std::cout << "matrix exponentiation with e=0 test failed: " << e.what() << "\n";
    }

    std::cout << "All DynamicMatrix unit tests completed!\n";

    return 0;
}
