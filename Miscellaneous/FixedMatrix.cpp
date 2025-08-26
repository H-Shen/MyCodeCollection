#include <array>
#include <iostream>
#include <type_traits>
#include <cassert>

// General fixed-size matrix with R rows and C columns, C++23
// - Supports addition/subtraction for same-shape matrices
// - Supports multiplication for Mat<R,C> * Mat<C,K> = Mat<R,K>
// - Supports scalar multiply and exponentiation (square matrices only)
// - Provides both modular and non-modular interfaces
// - T can be integral or floating point
// - R,C > 0

template<typename T, std::size_t R, std::size_t C> requires std::is_arithmetic_v<T> && (R > 0) && (C > 0)
class Mat {
public:
    static constexpr std::size_t rows = R;
    static constexpr std::size_t cols = C;

    std::array<std::array<T, C>, R> A{};

    constexpr Mat() noexcept = default;

    // Element access
    [[nodiscard]] constexpr T *operator[](std::size_t i) {
        if (i >= R)
            throw std::out_of_range("Mat::operator[]: row index out of range");
        return A[i].data();
    }

    [[nodiscard]] constexpr const T *operator[](std::size_t i) const {
        if (i >= R)
            throw std::out_of_range("Mat::operator[]: row index out of range");
        return A[i].data();
    }

    // Non-modular addition/subtraction (requires same shape)
    [[nodiscard]] constexpr Mat add(const Mat &o) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = A[i][j] + o.A[i][j];
        return res;
    }

    [[nodiscard]] constexpr Mat sub(const Mat &o) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = A[i][j] - o.A[i][j];
        return res;
    }

    // Scalar multiplication
    [[nodiscard]] constexpr Mat mul(T s) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = A[i][j] * s;
        return res;
    }

    // Non-modular matrix multiplication: Mat<R,C> * Mat<C,K>
    template<std::size_t K>
    [[nodiscard]] constexpr Mat<T, R, K> mul(const Mat<T, C, K> &o) const noexcept {
        Mat<T, R, K> res;
        for (std::size_t i = 0; i != R; ++i) {
            for (std::size_t k = 0; k != C; ++k) {
                T t = A[i][k];
                for (std::size_t j = 0; j != K; ++j) {
                    res.A[i][j] += t * o.A[k][j];
                }
            }
        }
        return res;
    }

    // Exponentiation (square matrices only)
    template<std::size_t M = R>
    [[nodiscard]] constexpr Mat pow(uint64_t e) const noexcept requires (M == C) {
        Mat res = Mat::identity();
        Mat base = *this;
        while (e != 0) {
            if (e & 1) res = res.mul(base);
            base = base.mul(base);
            e >>= 1;
        }
        return res;
    }

    // Identity for square matrices
    template<std::size_t M = R>
    [[nodiscard]] static constexpr Mat identity() noexcept requires (M == C) {
        Mat I;
        for (std::size_t i = 0; i != R; ++i) I.A[i][i] = T(1);
        return I;
    }

    // Modular operations
    [[nodiscard]] constexpr Mat add(const Mat &o, T mod) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = mod_reduce(A[i][j] + o.A[i][j], mod);
        return res;
    }

    [[nodiscard]] constexpr Mat sub(const Mat &o, T mod) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = mod_reduce(A[i][j] - o.A[i][j], mod);
        return res;
    }

    [[nodiscard]] constexpr Mat mul(T s, T mod) const noexcept {
        Mat res;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                res.A[i][j] = mod_reduce(A[i][j] * s, mod);
        return res;
    }

    template<std::size_t K>
    [[nodiscard]] constexpr Mat<T, R, K> mul(const Mat<T, C, K> &o, T mod) const noexcept {
        Mat<T, R, K> res;
        for (std::size_t i = 0; i != R; ++i) {
            for (std::size_t k = 0; k != C; ++k) {
                T t = mod_reduce(A[i][k], mod);
                for (std::size_t j = 0; j != K; ++j) {
                    T p = mod_reduce(t * o.A[k][j], mod);
                    res.A[i][j] = mod_reduce(res.A[i][j] + p, mod);
                }
            }
        }
        return res;
    }

    template<std::size_t M = R>
    [[nodiscard]] constexpr Mat pow(uint64_t e, T mod) const noexcept requires (M == C) {
        Mat result = Mat::identity();
        Mat base;
        for (std::size_t i = 0; i != R; ++i)
            for (std::size_t j = 0; j != C; ++j)
                base.A[i][j] = mod_reduce(A[i][j], mod);
        while (e != 0) {
            if (e & 1) result = result.mul(base, mod);
            base = base.mul(base, mod);
            e >>= 1;
        }
        return result;
    }

    // Debug
    void debug_print() const {
        std::cout << "Mat<" << typeid(T).name()
                  << "," << R << "x" << C << ">" << '\n';
        for (auto const &row: A) {
            for (auto const &x: row) std::cout << x << ' ';
            std::cout << '\n';
        }
    }
private:
    static constexpr T mod_reduce(T x, T mod) noexcept {
        if constexpr (std::is_integral_v<T>) {
            assert(mod > 0);
            x %= mod;
            if (x < T(0)) x += mod;
            return x;
        } else {
            return x;
        }
    }
};

// Unit tests for Mat public interfaces
template<typename M>
void assert_eq(const M &a, const M &b, const std::string &msg = "") {
    using T = std::remove_cvref_t<decltype(a[0][0])>;
    static_assert(std::is_arithmetic_v<T>);
    for (std::size_t i = 0; i != M::rows; ++i)
        for (std::size_t j = 0; j != M::cols; ++j)
            assert(a[i][j] == b[i][j] && msg.c_str());
}

int main() {
    // 1. Test non-modular addition and subtraction
    Mat<int, 2, 3> m1;
    m1[0][0] = 1;
    m1[0][1] = 2;
    m1[0][2] = 3;
    m1[1][0] = 4;
    m1[1][1] = 5;
    m1[1][2] = 6;
    Mat<int, 2, 3> m2;
    m2[0][0] = 6;
    m2[0][1] = 5;
    m2[0][2] = 4;
    m2[1][0] = 3;
    m2[1][1] = 2;
    m2[1][2] = 1;
    Mat<int, 2, 3> sum_expected;
    sum_expected[0][0] = 7;
    sum_expected[0][1] = 7;
    sum_expected[0][2] = 7;
    sum_expected[1][0] = 7;
    sum_expected[1][1] = 7;
    sum_expected[1][2] = 7;
    assert_eq(m1.add(m2), sum_expected, "addition failed");
    Mat<int, 2, 3> diff_expected;
    diff_expected[0][0] = -5;
    diff_expected[0][1] = -3;
    diff_expected[0][2] = -1;
    diff_expected[1][0] = 1;
    diff_expected[1][1] = 3;
    diff_expected[1][2] = 5;
    assert_eq(m1.sub(m2), diff_expected, "subtraction failed");

    // 2. Test scalar multiplication
    Mat<int, 2, 3> scaled_expected;
    scaled_expected[0][0] = 2;
    scaled_expected[0][1] = 4;
    scaled_expected[0][2] = 6;
    scaled_expected[1][0] = 8;
    scaled_expected[1][1] = 10;
    scaled_expected[1][2] = 12;
    assert_eq(m1.mul(2), scaled_expected, "mul failed");

    // 3. Test matrix multiplication
    Mat<int, 3, 2> ma;
    ma[0][0] = 1;
    ma[0][1] = 2;
    ma[1][0] = 3;
    ma[1][1] = 4;
    ma[2][0] = 5;
    ma[2][1] = 6;
    Mat<int, 2, 4> mb;
    mb[0][0] = 1;
    mb[0][1] = 0;
    mb[0][2] = 2;
    mb[0][3] = 3;
    mb[1][0] = 4;
    mb[1][1] = 5;
    mb[1][2] = 6;
    mb[1][3] = 7;
    // compute ma * mb
    Mat<int, 3, 4> prod_expected;
    // row0
    prod_expected[0][0] = 1 * 1 + 2 * 4;
    prod_expected[0][1] = 1 * 0 + 2 * 5;
    prod_expected[0][2] = 1 * 2 + 2 * 6;
    prod_expected[0][3] = 1 * 3 + 2 * 7;
    // row1
    prod_expected[1][0] = 3 * 1 + 4 * 4;
    prod_expected[1][1] = 3 * 0 + 4 * 5;
    prod_expected[1][2] = 3 * 2 + 4 * 6;
    prod_expected[1][3] = 3 * 3 + 4 * 7;
    // row2
    prod_expected[2][0] = 5 * 1 + 6 * 4;
    prod_expected[2][1] = 5 * 0 + 6 * 5;
    prod_expected[2][2] = 5 * 2 + 6 * 6;
    prod_expected[2][3] = 5 * 3 + 6 * 7;
    assert_eq(ma.mul(mb), prod_expected, "matrix multiplication failed");

    // 4. Test exponentiation non-modular
    Mat<int, 2, 2> p;
    p[0][0] = 1;
    p[0][1] = 1;
    p[1][0] = 1;
    p[1][1] = 0;
    auto p2 = p.pow(5);
    // Fibonacci matrix^5 = [[F6,F5],[F5,F4]] = [[8,5],[5,3]]
    Mat<int, 2, 2> fib_expect;
    fib_expect[0][0] = 8;
    fib_expect[0][1] = 5;
    fib_expect[1][0] = 5;
    fib_expect[1][1] = 3;
    assert_eq(p2, fib_expect, "pow failed");

    // 5. Test modular add, mul, pow
    Mat<int, 2, 2> ma_mod = p;
    auto add_mod_res = ma_mod.add(ma_mod, 100);
    Mat<int, 2, 2> add_mod_expect = p.add(p);
    assert_eq(add_mod_res, add_mod_expect, "add mod failed");
    auto mul_mod_res = ma_mod.mul(ma_mod, 100);
    Mat<int, 2, 2> mul_mod_expect;
    // p*p = [[2,1],[1,1]]
    mul_mod_expect[0][0] = 2;
    mul_mod_expect[0][1] = 1;
    mul_mod_expect[1][0] = 1;
    mul_mod_expect[1][1] = 1;
    assert_eq(mul_mod_res, mul_mod_expect, "mul mod failed");
    auto pow_mod_res = p.pow(3, 100);
    // p^3 = [[3,2],[2,1]]
    Mat<int, 2, 2> pow_mod_expect;
    pow_mod_expect[0][0] = 3;
    pow_mod_expect[0][1] = 2;
    pow_mod_expect[1][0] = 2;
    pow_mod_expect[1][1] = 1;
    assert_eq(pow_mod_res, pow_mod_expect, "pow mod failed");

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
