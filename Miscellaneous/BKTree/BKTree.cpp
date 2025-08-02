#include "BKTree.h"
#include "Utility.hpp"   // lowercase include for utility header
#include <limits>

// Note: Constructors, move/copy operators, and destructor are defaulted inline in BKTree.h.
// No manual delete_node is needed because std::unique_ptr handles node cleanup.

// Public insert: initializes root if empty, otherwise delegates
void BKTree::insert(const std::string& word) {
    if (!root_) {
        root_ = std::make_unique<Node>(word);
        ++node_count_;
    } else {
        insert_node(root_.get(), word);
    }
}

// Internal insert: compute distance and attach as child or recurse
void BKTree::insert_node(Node* node, const std::string& word) {
    int dist = Utility::levenshtein_distance(node->word, word);
    if (dist == 0) return;  // ignore exact duplicates

    auto it = node->children.find(dist);
    if (it != node->children.end()) {
        insert_node(it->second.get(), word);
    } else {
        node->children[dist] = std::make_unique<Node>(word);
        ++node_count_;
    }
}

// Replace dictionary: clear existing tree and insert unique words
void BKTree::set_dictionary(const std::vector<std::string>& words) {
    std::unordered_set<std::string> unique_words(words.begin(), words.end());
    root_.reset();
    node_count_ = 0;
    for (const auto& w : unique_words) {
        insert(w);
    }
}

// Public find: throws if empty, then performs search
bool BKTree::find_closest_word(const std::string& word,
                               std::string& closest,
                               int& best_dist) const {
    if (!root_) throw std::runtime_error("BKTree not initialized");
    best_dist = std::numeric_limits<int>::max();
    find_closest_word(root_.get(), word, closest, best_dist);
    return best_dist != std::numeric_limits<int>::max();
}

// Internal find: update best match and recurse within dynamic bounds
void BKTree::find_closest_word(const Node* node,
                               const std::string& word,
                               std::string& closest,
                               int& best_dist) const {
    int dist = Utility::levenshtein_distance(node->word, word);
    if (dist < best_dist) {
        best_dist = dist;
        closest = node->word;
    }

    // Compute dynamic pruning range based on current best
    int lower = dist - best_dist;
    int upper = dist + best_dist;
    for (const auto& [d, child] : node->children) {
        if (d < lower || d > upper) continue;
        find_closest_word(child.get(), word, closest, best_dist);
    }
}
