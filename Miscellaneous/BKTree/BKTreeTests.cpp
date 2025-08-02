#include "BKTree.h"
#include "Utility.hpp"
#include <iostream>
#include <cassert>

// Test: find_closest_word should throw when tree is empty
void testEmptyTree() {
    BKTree tree;
    std::string closest;
    int dist;
    bool exceptionThrown = false;
    try {
        tree.find_closest_word("test", closest, dist);
    } catch (const std::runtime_error&) {
        exceptionThrown = true;
    }
    assert(exceptionThrown && "find_closest_word should throw on empty tree");
}

// Test: insert and size accounting (duplicates ignored)
void testInsertAndSize() {
    BKTree tree;
    tree.insert("hello");
    tree.insert("world");
    tree.insert("hello");  // duplicate
    assert(tree.size() == 2 && "Duplicate insert should not increase size");
}

// Test: find exact match
void testFindClosestExact() {
    BKTree tree;
    tree.set_dictionary({"apple", "banana", "cherry"});
    std::string res;
    int d;
    bool found = tree.find_closest_word("banana", res, d);
    assert(found && res == "banana" && d == 0);
}

// Test: fuzzy match
void testFindClosestFuzzy() {
    BKTree tree;
    tree.set_dictionary({"kitten", "sitting", "bitten"});
    std::string res;
    int d;
    bool found;

    found = tree.find_closest_word("kitten", res, d);
    assert(found && res == "kitten" && d == 0);

    found = tree.find_closest_word("kittne", res, d);
    assert(found && res == "kitten" && d == 2);
}

int main() {
    testEmptyTree();
    testInsertAndSize();
    testFindClosestExact();
    testFindClosestFuzzy();
    std::cout << "All BKTree tests passed!" << std::endl;
    return 0;
}
