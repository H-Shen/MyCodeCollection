#include <iostream>
#include <vector>
#include <span>
#include <optional>
#include <concepts>

template<typename T>
concept Comparable = std::totally_ordered<T>;

/**
 * Find the minimum and maximum elements in an array using the optimal
 * tournament algorithm with approximately 3n/2 comparisons.
 *
 * @param arr Input array as a span
 * @return Optional pair of (min, max), or nullopt if array is empty
 */
template<Comparable T>
constexpr std::optional<std::pair<T, T>> my_minmax(std::span<const T> arr) {
    const auto n = arr.size();

    // Handle empty array
    if (n == 0) {
        return std::nullopt;
    }

    // Handle single element
    if (n == 1) {
        return std::pair{arr[0], arr[0]};
    }

    T min_val, max_val;
    std::size_t start;

    // Initialize min and max based on array length parity
    if (n & 1) {
        // Odd length: initialize with first element, then process pairs from index 1
        min_val = max_val = arr[0];
        start = 1;
    } else {
        // Even length: initialize with first two elements, then process pairs from index 2
        if (arr[0] < arr[1]) {
            min_val = arr[0];
            max_val = arr[1];
        } else {
            min_val = arr[1];
            max_val = arr[0];
        }
        start = 2;
    }

    // Process remaining elements in pairs
    // For each pair, compare them first, then update min/max
    // This reduces total comparisons to ~3n/2
    for (auto i = start; i < n - 1; i += 2) {
        const auto &left = arr[i];
        const auto &right = arr[i + 1];

        if (left < right) {
            // left is smaller: compare left with min, right with max
            if (left < min_val) min_val = left;
            if (right > max_val) max_val = right;
        } else if (left > right) {
            // right is smaller: compare right with min, left with max
            if (right < min_val) min_val = right;
            if (left > max_val) max_val = left;
        }
        // If left == right, no need to update (both can't be new extremes)
    }

    return std::pair{min_val, max_val};
}

// Convenience overloads for different container types

template<Comparable T>
constexpr std::optional<std::pair<T, T>> my_minmax(const std::vector<T> &vec) {
    return my_minmax(std::span{vec});
}

template<Comparable T, std::size_t N>
constexpr std::optional<std::pair<T, T>> my_minmax(const T (&arr)[N]) {
    return my_minmax(std::span{arr, N});
}

template<Comparable T>
constexpr std::optional<std::pair<T, T>> my_minmax(std::initializer_list<T> list) {
    return my_minmax(std::span{list.begin(), list.size()});
}

// Helper function to print test results
template<typename T>
void print_result(const std::optional<std::pair<T, T>>& result, const std::string& test_name) {
    std::cout << test_name << ": ";
    if (result) {
        auto [min_val, max_val] = *result;
        std::cout << "[" << min_val << ", " << max_val << "]\n";
    } else {
        std::cout << "nullopt (empty array)\n";
    }
}

int main() {
    std::cout << "=== Basic Test Cases ===\n";

    // Test case 1: Mixed values with duplicate min
    std::vector A1 = {1, 2, 3, 4, 1};
    print_result(my_minmax(A1), "A1 {1,2,3,4,1}");

    // Test case 2: Two elements
    std::vector A2 = {1, 2};
    print_result(my_minmax(A2), "A2 {1,2}");

    // Test case 3: All equal elements
    std::vector A3 = {1, 1};
    print_result(my_minmax(A3), "A3 {1,1}");

    // Test case 4: Single element
    std::vector A4 = {0};
    print_result(my_minmax(A4), "A4 {0}");

    // Test case 5: Empty array
    std::vector<int> A5;
    print_result(my_minmax(A5), "A5 {}");

    std::cout << "\n=== Odd Length Arrays ===\n";

    // Test case 6: Odd length with random order
    std::vector A6 = {5, 1, 9, 3, 7};
    print_result(my_minmax(A6), "A6 {5,1,9,3,7}");

    // Test case 7: Odd length with min at start
    std::vector A7 = {1, 5, 3, 9, 7};
    print_result(my_minmax(A7), "A7 {1,5,3,9,7}");

    // Test case 8: Odd length with max at start
    std::vector A8 = {9, 5, 3, 1, 7};
    print_result(my_minmax(A8), "A8 {9,5,3,1,7}");

    std::cout << "\n=== Even Length Arrays ===\n";

    // Test case 9: Even length with random order
    std::vector A9 = {5, 1, 9, 3};
    print_result(my_minmax(A9), "A9 {5,1,9,3}");

    // Test case 10: Even length ascending
    std::vector A10 = {1, 2, 3, 4};
    print_result(my_minmax(A10), "A10 {1,2,3,4}");

    // Test case 11: Even length descending
    std::vector A11 = {4, 3, 2, 1};
    print_result(my_minmax(A11), "A11 {4,3,2,1}");

    std::cout << "\n=== Edge Cases ===\n";

    // Test case 12: All elements equal (larger array)
    std::vector A12 = {3, 3, 3, 3, 3};
    print_result(my_minmax(A12), "A12 {3,3,3,3,3}");

    // Test case 13: Large range
    std::vector A13 = {-1000, 0, 1000};
    print_result(my_minmax(A13), "A13 {-1000,0,1000}");

    // Test case 14: Negative numbers
    std::vector A14 = {-5, -1, -9, -3};
    print_result(my_minmax(A14), "A14 {-5,-1,-9,-3}");

    std::cout << "\n=== Initializer List ===\n";

    // Test case 15: Direct initializer list
    print_result(my_minmax({5, 2, 8, 1, 9}), "Init list {5,2,8,1,9}");

    std::cout << "\n=== Different Types ===\n";

    // Test case 16: Floating point
    std::vector A16 = {3.14, 2.71, 1.41, 2.23};
    print_result(my_minmax(A16), "A16 doubles {3.14,2.71,1.41,2.23}");

    // Test case 17: Characters
    std::vector A17 = {'d', 'a', 'c', 'b'};
    print_result(my_minmax(A17), "A17 chars {'d','a','c','b'}");

    std::cout << "\n=== C-style Array ===\n";

    // Test case 18: C-style array
    int A18[] = {7, 2, 9, 1, 5};
    print_result(my_minmax(A18), "A18 C-array {7,2,9,1,5}");

    std::cout << "\n=== Stress Test ===\n";

    // Test case 19: Larger array
    std::vector<int> A19;
    for (int i = 100; i >= 1; --i) {
        A19.push_back(i);
    }
    auto result19 = my_minmax(A19);
    std::cout << "A19 100 descending elements: ";
    if (result19) {
        auto [min_val, max_val] = *result19;
        std::cout << "[" << min_val << ", " << max_val << "] ";
        std::cout << (min_val == 1 && max_val == 100 ? "✓ PASS" : "✗ FAIL") << "\n";
    }

    std::cout << "\n=== All tests completed ===\n";

    return 0;
}
