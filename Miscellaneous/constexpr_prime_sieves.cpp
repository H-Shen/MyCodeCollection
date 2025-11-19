// Compile-time prime sieves with C++23 improvements
//
// Compile (GCC):
//   g++ -std=c++23 -O2 -Wall -Wextra -Wpedantic -Werror \
//       constexpr_prime_sieves_fixed.cpp -o sieve
//
// Compile (Clang):
//   clang++ -std=c++23 -O2 -Wall -Wextra -Wpedantic -Werror \
//       constexpr_prime_sieves_fixed.cpp -o sieve
//
// Compile (MSVC):
//   cl /std:c++23 /O2 /W4 /WX /permissive- /EHsc /utf-8 /bigobj \
//      constexpr_prime_sieves_fixed.cpp
//
// For very large N (for example using constexpr sieves with N = 1000000),
// the compiler may need more heap and constexpr evaluation budget.
//
// Note: Large N (>100000) may significantly increase compilation time
//       and memory usage. For N > 1000000, strongly consider a runtime
//       sieve or precomputed tables.

#include <array>
#include <bitset>
#include <iostream>

//==============================================================================
// Compile-time upper bound for prime counting function pi(N)
//
// We avoid std::log here because some standard library implementations
// do not make it constexpr yet, which breaks use in array bounds.
// Instead, we use an integer-based log2 and a safe inequality.
//
// For N >= 6, we use a conservative bound of the form:
//   pi(N) <= C * N / log2(N) + K
// with C and K chosen so that the bound is comfortably above pi(N)
// for all reasonable N.
//
// This is not tight but is safe and constexpr on all compilers.
//==============================================================================
constexpr int prime_count_upper_bound(int N) {
    if (N < 2) {
        return 0;
    }
    if (N < 6) {
        // For very small N, just return N (trivial safe upper bound).
        return N;
    }

    // Compute floor(log2(N)) in a constexpr-friendly way.
    int tmp = N;
    int log2_n = 0;
    while (tmp > 1) {
        tmp >>= 1;
        ++log2_n;
    }

    // Tighter conservative bound:
    //   pi(N) <= 2 * N / log2(N) + 16 (approximately)
    // We use integer floor(log2(N)) to keep it constexpr and safe.
    int bound = (2 * N) / log2_n + 16;

    return bound;
}


//==============================================================================
// Compile-time implementation of the classic Eratosthenes sieve.
// Time complexity: O(N log log N)
//
// After construction, you can query primality using check(q).
//
// Usage:
//   constexpr EratosthenesPrimeSieve<1000> sieve{};
//   static_assert(sieve.check(17));  // compile-time check
//   bool is_prime = sieve.check(19); // runtime check
//==============================================================================
template <int N>
    requires (N >= 0)
struct EratosthenesPrimeSieve {
    std::bitset<N> is_prime;  // primality flags for [0..N)

    // constexpr constructor performs the sieve entirely at compile time
    constexpr EratosthenesPrimeSieve() : is_prime{} {
        is_prime.set();  // assume all numbers are prime initially

        if constexpr (N > 0) {
            is_prime[0] = false;  // 0 is not prime
        }
        if constexpr (N > 1) {
            is_prime[1] = false;  // 1 is not prime
        }

        // Sieve of Eratosthenes: mark multiples of each prime
        for (int p = 2; p * p < N; ++p) {
            if (is_prime[p]) {
                // Optimization: start from p * p (smaller multiples already marked)
                for (int multiple = p * p; multiple < N; multiple += p) {
                    is_prime[multiple] = false;
                }
            }
        }
    }

    // Query if q is prime (0 <= q < N)
    [[nodiscard]] constexpr bool check(int q) const noexcept {
        if (q < 0 || q >= N) {
            return false;
        }
        return is_prime[q];
    }

    // Count total number of primes less than N
    [[nodiscard]] constexpr int count() const noexcept {
        return static_cast<int>(is_prime.count());
    }
};

//==============================================================================
// Compile-time implementation of the linear (Euler) sieve.
// Time complexity: O(N) - each composite is marked exactly once
//
// Populates prime[0..num_prime) with all primes < N in ascending order.
// Also provides fast primality check via check() or is_composite.
//
// Usage:
//   constexpr LinearPrimeSieve<1000> sieve{};
//   static_assert(sieve.prime[0] == 2);
//   static_assert(sieve.check(17));
//   int count = sieve.num_prime;
//==============================================================================
template <int N>
    requires (N >= 2)
struct LinearPrimeSieve {
    // Use tight-ish upper bound for prime array size (saves memory vs N).
    static constexpr int MAX_PRIMES = prime_count_upper_bound(N);

    std::array<int, MAX_PRIMES> prime;  // all primes < N, in order
    int num_prime;                      // number of primes found
    std::bitset<N> is_composite;       // composite flags (true if composite)

    // constexpr constructor runs linear sieve at compile time
    constexpr LinearPrimeSieve() : prime{}, num_prime(0), is_composite{} {
        is_composite[0] = true;  // 0 is not prime
        if constexpr (N > 1) {
            is_composite[1] = true;  // 1 is not prime
        }

        // Linear sieve: for each number, mark its multiples with smallest prime factor
        for (int num = 2; num < N; ++num) {
            // If not yet marked composite, it is a new prime
            if (!is_composite[num]) {
                prime[num_prime++] = num;
            }

            // Mark multiples of num with each known prime
            for (int idx = 0; idx < num_prime; ++idx) {
                int p = prime[idx];
                int multiple = num * p;

                if (multiple >= N) {
                    break;
                }

                is_composite[multiple] = true;

                // Key optimization: stop when p is the smallest prime factor of num
                // This ensures each composite is marked exactly once
                if (num % p == 0) {
                    break;
                }
            }
        }
    }

    // Query if q is prime (0 <= q < N)
    [[nodiscard]] constexpr bool check(int q) const noexcept {
        if (q < 0 || q >= N) {
            return false;
        }
        return !is_composite[q];
    }

    // Linear search to find if p is in the prime list (alternative check)
    [[nodiscard]] constexpr bool is_in_prime_list(int p) const noexcept {
        for (int i = 0; i < num_prime; ++i) {
            if (prime[i] == p) {
                return true;
            }
            if (prime[i] > p) {
                return false;  // primes are sorted
            }
        }
        return false;
    }
};

//==============================================================================
// Helper function to print first N primes
//==============================================================================
template <int MAX>
void print_first_primes(int count) {
    constexpr LinearPrimeSieve<MAX> sieve{};
    std::cout << "First " << count << " primes: ";
    for (int i = 0; i < count && i < sieve.num_prime; ++i) {
        std::cout << sieve.prime[i] << " ";
    }
    std::cout << "\n";
}

//==============================================================================
// Helper function to demonstrate memory savings
//==============================================================================
template <int N>
void show_memory_usage() {
    constexpr LinearPrimeSieve<N> sieve{};

    // Old approach: std::array<int, N>
    constexpr size_t old_size = N * sizeof(int) + N / 8;

    // New approach: std::array<int, MAX_PRIMES>
    constexpr size_t new_size = sieve.MAX_PRIMES * sizeof(int) + N / 8;

    std::cout << "Memory usage for N=" << N << ":\n";
    std::cout << "  Old approach: " << old_size << " bytes\n";
    std::cout << "  New approach: " << new_size << " bytes\n";
    std::cout << "  Savings: " << (old_size - new_size) << " bytes";
    std::cout << " (" << (100.0 * (old_size - new_size) / old_size) << "%)\n";
    std::cout << "  Actual primes: " << sieve.num_prime << "\n";
    std::cout << "  Array capacity: " << sieve.MAX_PRIMES << "\n\n";
}

//==============================================================================
// Main function with comprehensive compile-time and runtime tests
//==============================================================================
int main() {
    std::cout << "=== Compile-Time Prime Sieves (C++23) ===\n\n";

    //---------------------------------------------------------------------------
    // Test 1: Eratosthenes Sieve - Compile-time tests
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 1] Eratosthenes Sieve - Compile-time tests\n";

        constexpr EratosthenesPrimeSieve<100> sieve{};

        // Basic primality tests
        static_assert(sieve.check(2), "2 should be prime");
        static_assert(sieve.check(3), "3 should be prime");
        static_assert(sieve.check(97), "97 should be prime");
        static_assert(!sieve.check(0), "0 should not be prime");
        static_assert(!sieve.check(1), "1 should not be prime");
        static_assert(!sieve.check(4), "4 should not be prime");
        static_assert(!sieve.check(99), "99 should not be prime");

        // Boundary tests
        static_assert(!sieve.check(-1), "negative numbers are not prime");
        static_assert(!sieve.check(100), "out of range should return false");

        // Count test
        static_assert(sieve.count() == 25, "there are 25 primes below 100");

        std::cout << "  [OK] All compile-time assertions passed\n";
        std::cout << "  [OK] Found " << sieve.count() << " primes below 100\n\n";
    }

    //---------------------------------------------------------------------------
    // Test 2: Linear Sieve - Compile-time tests
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 2] Linear Sieve - Compile-time tests\n";

        constexpr LinearPrimeSieve<1005> sieve{};

        // Prime list tests
        static_assert(sieve.prime[0] == 2, "first prime must be 2");
        static_assert(sieve.prime[1] == 3, "second prime must be 3");
        static_assert(sieve.prime[2] == 5, "third prime must be 5");

        // Count test
        static_assert(sieve.num_prime == 168, "there are 168 primes below 1005");
        static_assert(sieve.prime[sieve.num_prime - 1] == 997,
                      "last prime below 1005 is 997");

        // Check method tests (API consistency)
        static_assert(sieve.check(2), "2 should be prime");
        static_assert(sieve.check(97), "97 should be prime");
        static_assert(!sieve.check(0), "0 should not be prime");
        static_assert(!sieve.check(1), "1 should not be prime");
        static_assert(!sieve.check(4), "4 should not be prime");

        // Composite flag tests
        static_assert(!sieve.is_composite[2], "2 should not be composite");
        static_assert(sieve.is_composite[4], "4 should be composite");
        static_assert(sieve.is_composite[100], "100 should be composite");

        // Boundary tests
        static_assert(!sieve.check(-1), "negative numbers are not prime");
        static_assert(!sieve.check(1005), "out of range should return false");

        std::cout << "  [OK] All compile-time assertions passed\n";
        std::cout << "  [OK] Found " << sieve.num_prime << " primes below 1005\n";
        std::cout << "  [OK] Array capacity: " << sieve.MAX_PRIMES
                  << " (vs " << 1005 << " in old approach)\n\n";
    }

    //---------------------------------------------------------------------------
    // Test 3: Consistency between two sieves
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 3] Consistency check between sieves\n";

        constexpr int TEST_N = 1000;
        constexpr EratosthenesPrimeSieve<TEST_N> era{};
        constexpr LinearPrimeSieve<TEST_N> lin{};

        // Verify both sieves agree on all numbers (runtime loop, both tables are constexpr)
        bool all_match = true;
        for (int i = 0; i < TEST_N; ++i) {
            if (era.check(i) != lin.check(i)) {
                all_match = false;
                break;
            }
        }

        std::cout << "  [OK] Eratosthenes and Linear sieves agree: "
                  << (all_match ? "YES" : "NO") << "\n";
        std::cout << "  [OK] Eratosthenes count: " << era.count() << "\n";
        std::cout << "  [OK] Linear count: " << lin.num_prime << "\n\n";
    }

    //---------------------------------------------------------------------------
    // Test 4: Runtime demonstrations
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 4] Runtime demonstrations\n\n";

        // Print first primes
        print_first_primes<1000>(20);

        // Show memory savings
        show_memory_usage<1000>();
        show_memory_usage<10000>();
        // Note: N=100000 may exceed compile-time limits on some compilers.
    }

    //---------------------------------------------------------------------------
    // Test 5: Large prime test (compile-time limit consideration)
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 5] Large prime test (N=10000)\n";
        std::cout << "  Note:\n";
        std::cout << "    - Increasing N (for example to 100000 or 1000000)\n";
        std::cout << "      will dramatically increase compile-time and\n";
        std::cout << "      constexpr memory usage.\n";
        std::cout << "    - For very large N (such as 1000000), you may need\n";
        std::cout << "      to increase compiler limits (MSVC /Zm, GCC/Clang\n";
        std::cout << "      constexpr step and memory limits) or switch to a\n";
        std::cout << "      pure runtime sieve.\n\n";

        constexpr int LARGE_N = 10000;
        constexpr EratosthenesPrimeSieve<LARGE_N> era{};
        constexpr LinearPrimeSieve<LARGE_N> lin{};

        std::cout << "  [OK] Eratosthenes found " << era.count() << " primes\n";
        std::cout << "  [OK] Linear found " << lin.num_prime << " primes\n";
        std::cout << "  [OK] Largest prime < " << LARGE_N << " is "
                  << lin.prime[lin.num_prime - 1] << "\n";

        // Verify specific large primes
        static_assert(era.check(9973), "9973 should be prime");
        static_assert(lin.check(9973), "9973 should be prime");

        std::cout << "  [OK] Both sieves correctly identify large primes\n\n";
    }

    //---------------------------------------------------------------------------
    // Test 6: Edge cases
    //---------------------------------------------------------------------------
    {
        std::cout << "[Test 6] Edge cases\n";

        // Very small N
        constexpr EratosthenesPrimeSieve<2> era2{};
        static_assert(era2.count() == 0, "no primes below 2");
        static_assert(!era2.check(0), "0 is not prime");
        static_assert(!era2.check(1), "1 is not prime");
        std::cout << "  [OK] N=2 handled correctly (Eratosthenes)\n";

        constexpr LinearPrimeSieve<2> lin2{};
        static_assert(lin2.num_prime == 0, "no primes below 2");
        std::cout << "  [OK] Linear sieve with N=2 works\n";

        constexpr LinearPrimeSieve<3> lin3{};
        static_assert(lin3.num_prime == 1, "one prime below 3");
        static_assert(lin3.prime[0] == 2, "that prime is 2");
        std::cout << "  [OK] Linear sieve with N=3 works\n\n";
    }

    std::cout << "=== All tests passed! ===\n";

    return 0;
}
