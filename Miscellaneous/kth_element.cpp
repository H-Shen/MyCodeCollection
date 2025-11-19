// kth_element
// Quick Select Algorithm - Find k-th smallest element in unsorted array
// Reference: CLRS "Introduction to Algorithms" Chapter 9
// 
// Time Complexity: O(n) average, O(n^2) worst case
// Space Complexity: O(1) for in-place version, O(n) for copying version

#include <vector>
#include <cassert>
#include <algorithm>
#include <random>
#include <iostream>

namespace QuickSelect {

/**
 * @brief Find k-th smallest element in array (0-indexed, in-place)
 * 
 * This function modifies the input array. After execution, A[k] contains
 * the k-th smallest element, but the array is not fully sorted.
 * 
 * @param A Input array (will be modified)
 * @param k Index of desired element (0 = smallest, n-1 = largest)
 * @return The k-th smallest element
 * 
 * Example:
 *   A = {3, 1, 4, 1, 5}
 *   kth_element_inplace(A, 2) returns 3
 *   After call, A might be {1, 1, 3, 4, 5} or {1, 1, 3, 5, 4}
 *   (only A[2] is guaranteed to be in final position)
 */
template<typename T>
T kth_element_inplace(std::vector<T>& A, int k) {
    int n = static_cast<int>(A.size());
    assert(k >= 0 && k < n && "k must be in range [0, n)");
    
    int l = 0;
    int r = n - 1;
    
    while (l < r) {
        // For small subarrays, use simple sort
        if (r - l <= 1) {
            if (A[l] > A[r]) {
                std::swap(A[l], A[r]);
            }
            return A[k];
        }
        
        // Median-of-three pivot selection (improves worst-case behavior)
        int mid = l + (r - l) / 2;
        
        // Sort A[l], A[mid], A[r] to get median
        if (A[l] > A[mid]) std::swap(A[l], A[mid]);
        if (A[mid] > A[r]) std::swap(A[mid], A[r]);
        if (A[l] > A[mid]) std::swap(A[l], A[mid]);
        
        // Now A[l] <= A[mid] <= A[r], use A[mid] as pivot
        // Move pivot to second position
        std::swap(A[mid], A[l + 1]);
        T pivot = A[l + 1];
        
        // Hoare partition scheme
        int i = l + 1;
        int j = r;
        
        while (true) {
            // Find element >= pivot from left
            do {
                ++i;
            } while (i <= r && A[i] < pivot);
            
            // Find element <= pivot from right
            do {
                --j;
            } while (j >= l && A[j] > pivot);
            
            // Pointers crossed, partitioning done
            if (i > j) break;
            
            std::swap(A[i], A[j]);
        }
        
        // Place pivot in its final position
        A[l + 1] = A[j];
        A[j] = pivot;
        
        // Now A[l..j-1] <= A[j] <= A[j+1..r]
        if (k < j) {
            r = j - 1;
        } else if (k > j) {
            l = j + 1;
        } else {
            return A[j];
        }
    }
    
    return A[k];
}

/**
 * @brief Find k-th smallest element (non-modifying version)
 * 
 * This function does not modify the input array. It creates a copy
 * internally, so it uses O(n) extra space.
 * 
 * @param A Input array (not modified)
 * @param k Index of desired element
 * @return The k-th smallest element
 */
template<typename T>
T kth_element(const std::vector<T>& A, int k) {
    std::vector<T> copy = A;
    return kth_element_inplace(copy, k);
}

/**
 * @brief Find k-th smallest element with randomized pivot (better average case)
 * 
 * Uses randomized pivot selection instead of median-of-three.
 * This gives better average-case performance and avoids worst-case
 * behavior on specific input patterns.
 * 
 * @param A Input array (will be modified)
 * @param k Index of desired element
 * @return The k-th smallest element
 */
template<typename T>
T kth_element_randomized(std::vector<T>& A, int k) {
    int n = static_cast<int>(A.size());
    assert(k >= 0 && k < n && "k must be in range [0, n)");
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    int l = 0;
    int r = n - 1;
    
    while (l < r) {
        if (r - l <= 1) {
            if (A[l] > A[r]) {
                std::swap(A[l], A[r]);
            }
            return A[k];
        }
        
        // Random pivot selection
        std::uniform_int_distribution<> dis(l, r);
        int pivot_idx = dis(gen);
        std::swap(A[pivot_idx], A[l]);
        
        // Hoare partition
        T pivot = A[l];
        int i = l;
        int j = r + 1;
        
        while (true) {
            do {
                ++i;
            } while (i <= r && A[i] < pivot);
            
            do {
                --j;
            } while (A[j] > pivot);
            
            if (i >= j) break;
            
            std::swap(A[i], A[j]);
        }
        
        std::swap(A[l], A[j]);
        
        // Recursively search correct partition
        if (j > k) {
            r = j - 1;
        } else if (j < k) {
            l = j + 1;
        } else {
            return A[k];
        }
    }
    
    return A[k];
}

/**
 * @brief Find k-th largest element (convert to k-th smallest)
 * 
 * @param A Input array (will be modified)
 * @param k Index from largest (0 = largest, n-1 = smallest)
 * @return The k-th largest element
 */
template<typename T>
T kth_largest(std::vector<T>& A, int k) {
    int n = static_cast<int>(A.size());
    assert(k >= 0 && k < n && "k must be in range [0, n)");
    return kth_element_inplace(A, n - 1 - k);
}

/**
 * @brief Find median of array
 * 
 * For odd-length arrays, returns the middle element.
 * For even-length arrays, returns the upper median (element at index n/2).
 * 
 * @param A Input array (not modified)
 * @return The median element
 */
template<typename T>
T median(const std::vector<T>& A) {
    int n = static_cast<int>(A.size());
    assert(n > 0 && "Array must not be empty");
    return kth_element(A, n / 2);
}

} // namespace QuickSelect

// ======================= Tests =======================

void test_basic() {
    std::cout << "[Test 1] Basic operations...\n";
    
    std::vector<int> A = {1, 3, 5, 2, 2};
    assert(QuickSelect::kth_element(A, 0) == 1);
    assert(QuickSelect::kth_element(A, 1) == 2);
    assert(QuickSelect::kth_element(A, 2) == 2);
    assert(QuickSelect::kth_element(A, 3) == 3);
    assert(QuickSelect::kth_element(A, 4) == 5);
    
    std::cout << "  [OK] Basic k-th element works\n";
}

void test_edge_cases() {
    std::cout << "[Test 2] Edge cases...\n";
    
    // All equal elements
    std::vector<int> A1 = {2, 2, 2, 2, 2};
    assert(QuickSelect::kth_element(A1, 0) == 2);
    assert(QuickSelect::kth_element(A1, 2) == 2);
    assert(QuickSelect::kth_element(A1, 4) == 2);
    std::cout << "  [OK] All equal elements\n";
    
    // Already sorted (ascending)
    std::vector<int> A2 = {1, 2, 3, 4, 5};
    assert(QuickSelect::kth_element(A2, 0) == 1);
    assert(QuickSelect::kth_element(A2, 2) == 3);
    assert(QuickSelect::kth_element(A2, 4) == 5);
    std::cout << "  [OK] Sorted array (ascending)\n";
    
    // Reverse sorted (descending)
    std::vector<int> A3 = {5, 4, 3, 2, 1};
    assert(QuickSelect::kth_element(A3, 0) == 1);
    assert(QuickSelect::kth_element(A3, 2) == 3);
    assert(QuickSelect::kth_element(A3, 4) == 5);
    std::cout << "  [OK] Sorted array (descending)\n";
    
    // Single element
    std::vector<int> A4 = {42};
    assert(QuickSelect::kth_element(A4, 0) == 42);
    std::cout << "  [OK] Single element\n";
    
    // Two elements
    std::vector<int> A5 = {2, 1};
    assert(QuickSelect::kth_element(A5, 0) == 1);
    assert(QuickSelect::kth_element(A5, 1) == 2);
    std::cout << "  [OK] Two elements\n";
    
    // Many duplicates
    std::vector<int> A6 = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    assert(QuickSelect::kth_element(A6, 5) == 4);
    std::cout << "  [OK] Many duplicates\n";
}

void test_negative_numbers() {
    std::cout << "[Test 3] Negative numbers...\n";
    
    std::vector<int> A = {-5, 3, -1, 0, 2, -3};
    assert(QuickSelect::kth_element(A, 0) == -5);
    assert(QuickSelect::kth_element(A, 1) == -3);
    assert(QuickSelect::kth_element(A, 2) == -1);
    assert(QuickSelect::kth_element(A, 3) == 0);
    assert(QuickSelect::kth_element(A, 4) == 2);
    assert(QuickSelect::kth_element(A, 5) == 3);
    
    std::cout << "  [OK] Negative numbers work\n";
}

void test_inplace_vs_copy() {
    std::cout << "[Test 4] In-place vs copy...\n";
    
    std::vector<int> original = {3, 1, 4, 1, 5, 9, 2, 6};
    std::vector<int> copy = original;
    
    // Non-modifying version
    int result1 = QuickSelect::kth_element(original, 3);
    assert(original == copy);  // Original unchanged
    std::cout << "  [OK] Non-modifying version preserves input\n";
    
    // In-place version
    int result2 = QuickSelect::kth_element_inplace(copy, 3);
    assert(result1 == result2);
    assert(copy[3] == result2);  // Element at k is correct
    assert(original != copy);    // Array was modified
    std::cout << "  [OK] In-place version modifies input\n";
}

void test_kth_largest() {
    std::cout << "[Test 5] k-th largest...\n";
    
    std::vector<int> A = {3, 1, 4, 1, 5, 9, 2, 6};
    
    // k-th largest should give reverse order
    assert(QuickSelect::kth_largest(A, 0) == 9);  // 1st largest
    assert(QuickSelect::kth_largest(A, 1) == 6);  // 2nd largest
    assert(QuickSelect::kth_largest(A, 2) == 5);  // 3rd largest
    
    std::cout << "  [OK] k-th largest works\n";
}

void test_median() {
    std::cout << "[Test 6] Median...\n";
    
    // Odd length
    std::vector<int> A1 = {3, 1, 4, 1, 5};
    assert(QuickSelect::median(A1) == 3);
    std::cout << "  [OK] Median (odd length)\n";
    
    // Even length: {1, 1, 3, 4}, n/2 = 2, so A[2] = 3
    std::vector<int> A2 = {3, 1, 4, 1};
    assert(QuickSelect::median(A2) == 3);
    std::cout << "  [OK] Median (even length)\n";
}

void test_large_array() {
    std::cout << "[Test 7] Large array...\n";
    
    // Generate large random array
    std::vector<int> A(100000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    
    for (int& x : A) {
        x = dis(gen);
    }
    
    // Find median using our implementation
    int our_median = QuickSelect::median(A);
    
    // Verify using std::nth_element
    std::vector<int> verify = A;
    std::nth_element(verify.begin(), verify.begin() + verify.size() / 2, verify.end());
    int std_median = verify[verify.size() / 2];
    
    assert(our_median == std_median);
    std::cout << "  [OK] Large array (100k elements) matches std::nth_element\n";
}

void test_randomized_version() {
    std::cout << "[Test 8] Randomized pivot selection...\n";
    
    std::vector<int> A = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    
    for (int i = 0; i < 10; ++i) {
        std::vector<int> copy = A;
        int result = QuickSelect::kth_element_randomized(copy, 5);
        // 6th smallest (index 5) is 4
        assert(result == 4);
    }
    
    std::cout << "  [OK] Randomized version works\n";
}

void test_comparison_with_std() {
    std::cout << "[Test 9] Comparison with std::nth_element...\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10000);
    
    for (int test = 0; test < 100; ++test) {
        // Generate random array
        int n = dis(gen) % 1000 + 1;
        std::vector<int> A(n);
        for (int& x : A) {
            x = dis(gen);
        }
        
        // Test random k
        int k = dis(gen) % n;
        
        // Our implementation
        int our_result = QuickSelect::kth_element(A, k);
        
        // std::nth_element
        std::vector<int> verify = A;
        std::nth_element(verify.begin(), verify.begin() + k, verify.end());
        int std_result = verify[k];
        
        assert(our_result == std_result);
    }
    
    std::cout << "  [OK] 100 random tests match std::nth_element\n";
}

int main() {
    std::cout << "=== Quick Select Algorithm Tests ===\n\n";
    
    test_basic();
    test_edge_cases();
    test_negative_numbers();
    test_inplace_vs_copy();
    test_kth_largest();
    test_median();
    test_large_array();
    test_randomized_version();
    test_comparison_with_std();
    
    std::cout << "\n=== All tests passed! ===\n";
    
    return 0;
}
