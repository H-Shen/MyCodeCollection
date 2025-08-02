#include <string>
#include <map>
#include <memory>

// Represents a node in the BK-tree
struct Node {
    std::string word;  // The term stored at this node
    // Children nodes mapped by their edit distance from this node's word
    std::map<int, std::unique_ptr<Node>> children;

    explicit Node(std::string w) : word(std::move(w)) {}
};
