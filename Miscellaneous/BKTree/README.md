# BK-Tree C++ Implementation

## Overview

A **Burkhard–Keller Tree (BK-Tree)** is a data structure optimized for approximate string matching under a given distance metric—commonly used for spell checking, autocorrection, fuzzy search, and more. This repository provides a modern, efficient, and memory-safe C++20 implementation of a BK-Tree using the Levenshtein (edit) distance.

## Features

- **Space-optimized Levenshtein**: Computes edit distance in O(min(m, n)) memory using only two rolling arrays.
- \*\*Node management with \*\*\`\`: Automatic cleanup without manual deletion.
- \*\*Ordered children with \*\*\`\`: Ensures deterministic traversal and repeatable query results.
- **Dynamic pruning**: Narrows search bounds during lookup to skip irrelevant subtrees, boosting performance in practice.
- **Duplicate filtering**: Ignores exact duplicates on insert and during bulk dictionary loading.
- **Exception safety**: Throws clear errors (e.g., lookup on empty tree) and documents behavior.

## Complexity Analysis

| API                       | Worst‑Case Time                            | Average‑Case Time                     | Space                                 |
| ------------------------- | ------------------------------------------ | ------------------------------------- | ------------------------------------- |
| `size()`                  | O(1)                                       | O(1)                                  | O(1)                                  |
| `insert(word)`            | O(N·L² + N·log N)                          |                                       |                                       |
| (worst chain)             | O(L²·log N + (log N)²)                     | O(L) (distance buffer)                |                                       |
| `set_dictionary(words)`   | O(W + ∑ inserts) ≈ O(W + U·(N·L²+N·log N)) | O(W + U·(L²·log N + (log N)²))        | O(U) (dedupe set) + O(L)              |
| `find_closest_word(word)` | O(N·L²)                                    | ≪ O(N·L²) (pruned; often O(L²·log N)) | O(h) recursion (≤ O(N), avg O(log N)) |

- **N** = number of nodes in the tree
- **L** = length of the processed word
- **W** = total words in input list for `set_dictionary`
- **U** = unique words after deduplication

## Prerequisites

- cmake (>=3.20)
- make (>=3.81)
- **C++20-compliant** compiler

## Building and Running

```shell
git clone https://github.com/H-Shen/BKTree.git
cd BKTree
rm -rf build
mkdir -p build
cd ./build
cmake ..
make
./output # Testing
```

All included unit tests verify empty-tree behavior, duplicate handling, exact matches, and fuzzy searches.

## Usage Example

```cpp
#include "BKTree.h"
#include <iostream>

int main() {
    BKTree tree;
    tree.set_dictionary({"apple", "banana", "orange", "grape"});

    std::string query = "appel";
    std::string closest;
    int distance;

    if (tree.find_closest_word(query, closest, distance)) {
        std::cout << "Did you mean '" << closest << "' (distance=" << distance << ")?\n";
    }
    return 0;
}
```

and run

```shell
g++ -std=c++20 -O2 -Wall -Werror -pthread -I. BKTree.cpp test.cpp -o bktree_test && ./bktree_test
```

