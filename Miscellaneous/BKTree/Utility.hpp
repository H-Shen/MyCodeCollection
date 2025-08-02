#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace Utility {

// Computes the Levenshtein (edit) distance between two strings
// Memory usage optimized to O(min(m, n)) by keeping only two rows
inline int levenshtein_distance(const std::string& s1, const std::string& s2) {
    // Ensure 'a' is the shorter string
    const std::string& a = s1.size() < s2.size() ? s1 : s2;
    const std::string& b = s1.size() < s2.size() ? s2 : s1;
    size_t n = a.size(), m = b.size();
    std::vector<int> prev(n + 1), curr(n + 1);

    // Initialize base case: distance from empty string
    for (size_t i = 0; i <= n; ++i) prev[i] = static_cast<int>(i);

    for (size_t j = 1; j <= m; ++j) {
        curr[0] = static_cast<int>(j);
        for (size_t i = 1; i <= n; ++i) {
            int cost = (a[i - 1] == b[j - 1] ? 0 : 1);
            curr[i] = std::min({ prev[i] + 1,        // deletion
                                 curr[i - 1] + 1,    // insertion
                                 prev[i - 1] + cost  // substitution
            });
        }
        std::swap(prev, curr);
    }
    return prev[n];
}

}  // namespace Utility