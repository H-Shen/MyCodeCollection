#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_set>

#include "Node.h"

// BKTree for approximate string matching using edit distance
class BKTree {
public:
    BKTree() = default;
    BKTree(const BKTree&) = delete;             // disable copy
    BKTree& operator=(const BKTree&) = delete;
    BKTree(BKTree&&) = default;                 // enable move
    BKTree& operator=(BKTree&&) = default;

    // Insert a word into the tree; duplicates are ignored
    void insert(const std::string& word);

    // Replace the dictionary with a new set of words (duplicates removed)
    void set_dictionary(const std::vector<std::string>& words);

    // Find the closest word to 'word'; returns true if found
    // 'closest' receives the best match and 'distance' receives its edit distance
    bool find_closest_word(const std::string& word, std::string& closest, int& distance) const;

    // Return the total number of nodes in the tree
    size_t size() const noexcept { return node_count_; }

private:
    // Recursive helpers
    void insert_node(Node* node, const std::string& word);
    void find_closest_word(const Node* node, const std::string& word,
                           std::string& closest, int& best_dist) const;

    std::unique_ptr<Node> root_;  // Root node of the BK-tree
    size_t node_count_{0};        // Number of nodes in the tree
};