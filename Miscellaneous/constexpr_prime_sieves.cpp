// g++ -std=c++23 -O2 -Wall -Werror constexpr_prime_sieves.cpp -o constexpr_prime_sieves
// clang++ -std=c++23 -O2 -Wall -Werror constexpr_prime_sieves.cpp -o constexpr_prime_sieves

#include <array>
#include <bitset>

//==============================================================================
// Compile-time implementation of the classic Eratosthenes sieve (time O(N log
// log N)). After construction, `is_prime[i] == true` iff `i` is a prime less
// than N. Usage:
//   constexpr EratosthenesPrimeSieve<MAXN> sieve{};
//   static_assert(sieve.check(17)); // compile-time prime check
//   bool runtime = sieve.check(19);
//==============================================================================
template <int N> struct EratosthenesPrimeSieve {
  std::bitset<N> is_prime; // bitset storing primality flags for [0..N)

  // constexpr constructor performs the sieve entirely at compile time
  constexpr EratosthenesPrimeSieve() : is_prime{} {
    is_prime.set(); // assume all numbers are prime
    if constexpr (N > 0)
      is_prime[0] = false; // 0 is not prime
    if constexpr (N > 1)
      is_prime[1] = false; // 1 is not prime

    // mark multiples of each prime starting from 2
    for (int p = 2; p * p < N; ++p) {
      if (is_prime[p]) {
        for (int m = p * p; m < N; m += p) {
          is_prime[m] = false; // composite
        }
      }
    }
  }

  // constexpr check function: return true if q is a prime (0 <= q < N)
  [[nodiscard]] constexpr bool check(int q) const {
    return q >= 0 && q < N && is_prime[q];
  }
};

//==============================================================================
// Compile-time implementation of the linear (Euler) sieve (time O(N)).
// Populates `prime[0..num_prime)` with all primes < N, and marks composites.
// Usage:
//   constexpr LinearPrimeSieve<MAXN> sieve{};
//   static_assert(sieve.prime[0] == 2);
//   int count = sieve.num_prime;
//==============================================================================
template <int N> struct LinearPrimeSieve {
  static_assert(N >= 2, "N must be >= 2");

  std::array<int, N> prime;    // stores all primes found, in ascending order
  int num_prime;               // number of primes stored
  std::bitset<N> is_not_prime; // composite flags: true if composite

  // constexpr constructor runs linear sieve at compile time
  constexpr LinearPrimeSieve() : prime{}, num_prime(0), is_not_prime{} {
    is_not_prime[0] = true;
    is_not_prime[1] = true;

    // for each i, if not yet marked composite, it's prime
    // then mark i * prime[j] as composite for each prime[j]
    for (int i = 2; i < N; ++i) {
      if (!is_not_prime[i]) {
        prime[num_prime++] = i; // record new prime
      }
      for (int j = 0; j < num_prime; ++j) {
        int p = prime[j];
        int ip = i * p;
        if (ip >= N)
          break;
        is_not_prime[ip] = true;
        if (i % p == 0)
          break; // ensure each composite marked once
      }
    }
  }
};

//==============================================================================
// Main function with compile-time tests for both sieves
//==============================================================================
int main() {
  {
    // Eratosthenes sieve tests
    constexpr EratosthenesPrimeSieve<100> sieve{};
    static_assert(sieve.check(2));   // 2 is prime
    static_assert(sieve.check(97));  // 97 is prime
    static_assert(!sieve.check(0));  // 0 is not prime
    static_assert(!sieve.check(1));  // 1 is not prime
    static_assert(!sieve.check(99)); // 99 is composite
  }

  {
    // Linear sieve tests
    constexpr LinearPrimeSieve<1005> sieve{};
    static_assert(sieve.prime[0] == 2, "first prime must be 2");
    static_assert(sieve.prime[1] == 3, "second prime must be 3");
    static_assert(sieve.num_prime >= 168, "at least 168 primes below 1000");
    static_assert(sieve.prime[sieve.num_prime - 1] == 997,
                  "last prime below 1005 must be 997");
    static_assert(!sieve.is_not_prime[2], "2 should not be marked composite");
    static_assert(sieve.is_not_prime[4], "4 should be marked composite");
  }

  return 0;
}
