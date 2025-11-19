#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace MPQ {

// Fixed-width aliases
using HashType    = std::uint32_t;
using BucketIndex = std::size_t;

// MPQ hash types
enum HashVariant : HashType {
    HASH_OFFSET = 0,
    HASH_A      = 1,
    HASH_B      = 2
};

// MPQ constants
constexpr HashType CRYPT_TABLE_LENGTH = 0x500;

// Growth & load-factor policy
constexpr BucketIndex INITIAL_BUCKETS = 8;
constexpr double      MAX_LOAD_FACTOR = 0.7;

/**
 * @brief Three hash values computed in a single pass.
 */
struct TripleHash {
    HashType offset;  // for bucket location
    HashType a;       // for key verification
    HashType b;       // for key verification
};

/**
 * @brief Compile-time generation of Blizzard's MPQ crypt table.
 */
consteval auto generate_crypt_table() -> std::array<HashType, CRYPT_TABLE_LENGTH> {
    std::array<HashType, CRYPT_TABLE_LENGTH> table{};
    HashType seed = 0x0010'0001;
    for (HashType i = 0; i < 0x100; ++i) {
        for (int j = 0; j < 5; ++j) {
            HashType idx = i + j * 0x100;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            HashType tmp1 = (seed & 0xFFFF) << 16;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            HashType tmp2 = (seed & 0xFFFF);
            table[idx] = tmp1 | tmp2;
        }
    }
    return table;
}
constexpr auto CRYPT_TABLE = generate_crypt_table();

/**
 * @brief Normalize a char to uppercase (locale-independent, no UB).
 * 
 * std::toupper has UB for negative char values. We avoid this by:
 * 1. Cast to unsigned char first
 * 2. Manual uppercase conversion for ASCII range
 */
constexpr auto normalize_char(char c) noexcept -> unsigned char {
    auto uc = static_cast<unsigned char>(c);
    // ASCII 'a'-'z' -> 'A'-'Z'
    if (uc >= 'a' && uc <= 'z') {
        return uc - ('a' - 'A');
    }
    return uc;
}

/**
 * @brief Case-insensitive string comparison for MPQ keys.
 */
inline bool keys_equal(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (normalize_char(a[i]) != normalize_char(b[i])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compute all three MPQ hashes in a single pass.
 * 
 * This eliminates redundant string traversal: instead of 3n character 
 * accesses, we do n accesses and compute 3 hashes simultaneously.
 * 
 * Implementation note: We manually unroll the three hash computations
 * rather than using an array + inner loop. This gives the compiler better
 * opportunities for register allocation and instruction-level parallelism.
 */
constexpr auto mpq_hash_all(std::string_view s) noexcept -> TripleHash {
    // Separate seed state for each hash type
    HashType seed1_0 = 0x7FED7FED, seed2_0 = 0xD34D10CC;
    HashType seed1_1 = 0x7FED7FED, seed2_1 = 0xD34D10CC;
    HashType seed1_2 = 0x7FED7FED, seed2_2 = 0xD34D10CC;
    
    for (char c : s) {
        const auto uc = normalize_char(c);
        
        // Hash type OFFSET (0 << 8 = 0)
        const HashType idx0 = uc;
        seed1_0 = CRYPT_TABLE[idx0] ^ (seed1_0 + seed2_0);
        seed2_0 = uc + seed1_0 + seed2_0 + (seed2_0 << 5) + 3;
        
        // Hash type A (1 << 8 = 0x100)
        const HashType idx1 = 0x100u | uc;
        seed1_1 = CRYPT_TABLE[idx1] ^ (seed1_1 + seed2_1);
        seed2_1 = uc + seed1_1 + seed2_1 + (seed2_1 << 5) + 3;
        
        // Hash type B (2 << 8 = 0x200)
        const HashType idx2 = 0x200u | uc;
        seed1_2 = CRYPT_TABLE[idx2] ^ (seed1_2 + seed2_2);
        seed2_2 = uc + seed1_2 + seed2_2 + (seed2_2 << 5) + 3;
    }
    
    return {seed1_0, seed1_1, seed1_2};
}

/**
 * @brief A single bucket entry.
 * 
 * Design choice: We use an explicit 'occupied' flag rather than relying on
 * hash sentinel values. While h_a==0 && h_b==0 has extremely low probability
 * (~1/2^64), it is NOT mathematically impossible. In library code, we must
 * guarantee zero probability of corruption, not "extremely low" probability.
 * 
 * Cost: 1 byte + padding (~8 bytes on 64-bit systems)
 * Benefit: Absolute correctness with zero probabilistic bugs
 */
template<typename T>
struct Node {
    bool        occupied{false};  // Explicit state
    HashType    h_a{0};           // Hash A
    HashType    h_b{0};           // Hash B
    std::string key;              // Empty for unused nodes
    T           value{};
    
    [[nodiscard]] constexpr bool is_empty() const noexcept {
        return !occupied;
    }
    
    void mark_empty() noexcept {
        occupied = false;
        h_a = 0;
        h_b = 0;
        key.clear();
    }
};

/**
 * @brief Open-addressing hash table specialized for MPQ string keys.
 * 
 * Uses linear probing for collision resolution. Load factor is kept below
 * 0.7 to maintain good average-case performance.
 */
template<typename T>
class HashTable {
public:
    explicit HashTable(BucketIndex buckets = INITIAL_BUCKETS)
      : _size(buckets), _buckets(buckets) {
        assert(buckets > 0 && "Bucket count must be positive");
    }

    /**
     * @brief Insert or update a key-value pair.
     * @return true on success, false if table is corrupted (should never happen)
     */
    bool put(std::string_view key, T value) {
        // Rehash before insertion if we'd exceed load factor
        if (static_cast<double>(_elements + 1) > _size * MAX_LOAD_FACTOR) {
            rehash(_size * 2);
        }
        return insert_impl(key, std::move(value));
    }

    /**
     * @brief Lookup a value by key (returns a copy).
     * @return std::optional with value if found, std::nullopt otherwise
     * 
     * Note: This copies the value. For large objects, prefer get_ref().
     */
    [[nodiscard]] auto get(std::string_view key) const noexcept -> std::optional<T> {
        auto pos = find_pos(key);
        if (!pos) return std::nullopt;
        return _buckets[*pos].value;
    }
    
    /**
     * @brief Lookup a value by key (returns a const reference, no copy).
     * @return Pointer to value if found, nullptr otherwise
     * 
     * Use this for large objects to avoid copying. The pointer is valid
     * until the next put() or rehash operation.
     */
    [[nodiscard]] auto get_ref(std::string_view key) const noexcept -> const T* {
        auto pos = find_pos(key);
        if (!pos) return nullptr;
        return &_buckets[*pos].value;
    }
    
    /**
     * @brief Lookup a value by key (returns a mutable reference, no copy).
     * @return Pointer to value if found, nullptr otherwise
     * 
     * Use this when you need to modify the value in-place. The pointer is
     * valid until the next put() or rehash operation.
     */
    [[nodiscard]] auto get_ref(std::string_view key) noexcept -> T* {
        auto pos = find_pos(key);
        if (!pos) return nullptr;
        return &_buckets[*pos].value;
    }

    [[nodiscard]] auto size()     const noexcept -> BucketIndex { return _elements; }
    [[nodiscard]] auto capacity() const noexcept -> BucketIndex { return _size; }
    
    /**
     * @brief Compute current load factor for diagnostics.
     */
    [[nodiscard]] auto load_factor() const noexcept -> double {
        return _size == 0 ? 0.0 : static_cast<double>(_elements) / _size;
    }

    /**
     * @brief Iterate over all occupied entries.
     */
    template<typename Func>
    void for_each(Func func) const {
        for (auto const& node : _buckets) {
            if (!node.is_empty()) {
                func(node.key, node.value);
            }
        }
    }

private:
    BucketIndex          _size{0};
    BucketIndex          _elements{0};
    std::vector<Node<T>> _buckets;

    /**
     * @brief Internal insertion without rehashing.
     * 
     * Logic is simplified into three clear cases:
     * 1. Empty slot found -> insert new entry
     * 2. Matching key found -> update value
     * 3. Hash collision -> continue probing
     */
    auto insert_impl(std::string_view key, T&& value) -> bool {
        auto [hm, ha, hb] = mpq_hash_all(key);
        auto idx = hm % _size;
        
        // Linear probing with wraparound
        for (BucketIndex probe = 0; probe < _size; ++probe) {
            auto& node = _buckets[idx];
            
            // Case 1: Found empty slot
            if (node.is_empty()) {
                node.occupied = true;
                node.h_a      = ha;
                node.h_b      = hb;
                node.key      = std::string(key);
                node.value    = std::move(value);
                ++_elements;
                return true;
            }
            
            // Case 2: Hash match - verify with actual key (collision check)
            if (node.h_a == ha && node.h_b == hb) {
                // MPQ is case-insensitive, so we compare normalized keys
                if (keys_equal(node.key, key)) {
                    // Key exists, update value
                    node.value = std::move(value);
                    return true;
                }
                // Hash collision (rare), continue probing
            }
            
            // Case 3: Occupied by different key, continue probing
            idx = (idx + 1) % _size;
        }
        
        // Table is full - should never happen due to load factor checks
        assert(false && "Hash table full despite load factor checks");
        return false;
    }

    /**
     * @brief Find bucket index for a given key (const version).
     * @return std::optional with index if found, std::nullopt otherwise
     */
    [[nodiscard]] auto find_pos(std::string_view key) const noexcept 
        -> std::optional<BucketIndex> 
    {
        if (_size == 0) return std::nullopt;
        
        auto [hm, ha, hb] = mpq_hash_all(key);
        auto idx = hm % _size;
        
        for (BucketIndex probe = 0; probe < _size; ++probe) {
            auto const& node = _buckets[idx];
            
            // Empty slot means key doesn't exist
            if (node.is_empty()) {
                return std::nullopt;
            }
            
            // Hash match - verify with actual key
            if (node.h_a == ha && node.h_b == hb && keys_equal(node.key, key)) {
                return idx;
            }
            
            idx = (idx + 1) % _size;
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief Find bucket index for a given key (non-const version).
     * @return std::optional with index if found, std::nullopt otherwise
     */
    [[nodiscard]] auto find_pos(std::string_view key) noexcept 
        -> std::optional<BucketIndex> 
    {
        if (_size == 0) return std::nullopt;
        
        auto [hm, ha, hb] = mpq_hash_all(key);
        auto idx = hm % _size;
        
        for (BucketIndex probe = 0; probe < _size; ++probe) {
            auto& node = _buckets[idx];
            
            // Empty slot means key doesn't exist
            if (node.is_empty()) {
                return std::nullopt;
            }
            
            // Hash match - verify with actual key
            if (node.h_a == ha && node.h_b == hb && keys_equal(node.key, key)) {
                return idx;
            }
            
            idx = (idx + 1) % _size;
        }
        
        return std::nullopt;
    }

    /**
     * @brief Grow the table and rehash all entries.
     * 
     * Straightforward approach: allocate new table, reinsert all entries.
     * Old table is automatically freed via std::vector.
     */
    void rehash(BucketIndex new_size) {
        assert(new_size > _size && "Rehash must grow the table");
        
        // Save old buckets
        auto old_buckets = std::move(_buckets);
        
        // Allocate new table
        _size     = new_size;
        _buckets  = std::vector<Node<T>>(new_size);
        _elements = 0;
        
        // Reinsert all entries
        for (auto& node : old_buckets) {
            if (!node.is_empty()) {
                // This should never fail since new table is larger
                bool ok = insert_impl(node.key, std::move(node.value));
                assert(ok && "Rehash insertion should never fail");
                (void)ok;  // Suppress unused warning in release builds
            }
        }
    }
};

} // namespace MPQ

// ======================= Tests =======================
int main() {
    std::cout << "=== MPQ Hash Table Tests (Refactored) ===\n\n";
    
    // --- Test 1: Basic operations ---
    {
        std::cout << "[Test 1] Basic operations...\n";
        MPQ::HashTable<int> table;
        assert(table.size() == 0);
        assert(table.capacity() >= 8);

        assert(table.put("orc",   10));
        assert(table.put("elf",   20));
        assert(table.put("human", 30));
        assert(table.put("orc",   40));  // overwrite

        auto o = table.get("orc");
        assert(o && *o == 40);
        assert(!table.get("dwarf"));

        std::cout << "  ✓ Insert, update, and lookup work correctly\n";
    }
    
    // --- Test 2: Load factor and rehashing ---
    {
        std::cout << "[Test 2] Load factor and rehashing...\n";
        MPQ::HashTable<int> table;
        
        for (int i = 0; i < 100; ++i) {
            assert(table.put("key" + std::to_string(i), i));
        }
        
        assert(table.size() == 100);
        assert(table.load_factor() < MPQ::MAX_LOAD_FACTOR);
        
        std::cout << "  ✓ Inserted 100 entries\n";
        std::cout << "  ✓ Final load factor: " << table.load_factor() << "\n";
    }
    
    // --- Test 3: Case insensitivity ---
    {
        std::cout << "[Test 3] Case insensitivity...\n";
        MPQ::HashTable<int> table;
        
        table.put("Hello", 1);
        table.put("HELLO", 2);
        table.put("hello", 3);
        
        // All should map to same key
        auto v = table.get("HeLLo");
        assert(v && *v == 3);
        assert(table.size() == 1);  // Only one entry
        
        std::cout << "  ✓ Case-insensitive hashing works\n";
    }
    
    // --- Test 4: Edge cases ---
    {
        std::cout << "[Test 4] Edge cases...\n";
        MPQ::HashTable<int> table;
        
        // Empty string
        assert(table.put("", 0));
        assert(table.get("") == 0);
        
        // Very long string
        std::string long_key(1000, 'x');
        assert(table.put(long_key, 999));
        assert(table.get(long_key) == 999);
        
        // Same character repeated
        assert(table.put("aaaaaaa", 7));
        assert(table.get("aaaaaaa") == 7);
        
        std::cout << "  ✓ Empty, long, and repetitive keys work\n";
    }

    // --- Test 5: Randomized stress test ---
    {
        std::cout << "[Test 5] Randomized stress test...\n";
        
        std::mt19937_64 rng{std::random_device{}()};
        std::uniform_int_distribution<int> len_dist(1, 16);
        std::uniform_int_distribution<int> char_dist('a', 'z');
        std::uniform_int_distribution<int> val_dist(0, 1'000'000);

        constexpr int N = 50'000;
        MPQ::HashTable<int> mpq_table;
        std::unordered_map<std::string, int> ref;

        // Generate and insert
        for (int i = 0; i < N; ++i) {
            const int len = len_dist(rng);
            std::string key;
            key.reserve(len);
            for (int j = 0; j < len; ++j) {
                key += static_cast<char>(char_dist(rng));
            }
            int v = val_dist(rng);

            // Convert to uppercase for reference (MPQ is case-insensitive)
            std::string normalized_key;
            normalized_key.reserve(key.size());
            for (char c : key) {
                normalized_key += static_cast<char>(MPQ::normalize_char(c));
            }
            
            ref[normalized_key] = v;
            if (!mpq_table.put(key, v)) {
                std::cerr << "  ✗ put() failed at insertion " << i << "\n";
                return 1;
            }
        }

        std::cout << "  ✓ Inserted " << N << " random keys\n";
        std::cout << "  ✓ Unique keys: " << ref.size() << "\n";
        std::cout << "  ✓ Load factor: " << mpq_table.load_factor() << "\n";

        // Verify all present keys
        for (auto const& [k, v] : ref) {
            if (auto o = mpq_table.get(k); !o || *o != v) {
                std::cerr << "  ✗ Mismatch for key \"" << k
                          << "\": expected " << v
                          << ", got " << (o ? std::to_string(*o) : "none")
                          << "\n";
                return 1;
            }
        }

        std::cout << "  ✓ All lookups matched reference map\n";

        // Test absent keys
        int absent_tested = 0;
        for (int i = 0; i < 1000; ++i) {
            std::string key = "zz_absent_" + std::to_string(i);
            std::string normalized_key;
            for (char c : key) {
                normalized_key += static_cast<char>(MPQ::normalize_char(c));
            }
            
            if (ref.contains(normalized_key)) continue;
            
            if (mpq_table.get(key)) {
                std::cerr << "  ✗ Unexpected hit for absent key \"" << key << "\"\n";
                return 1;
            }
            ++absent_tested;
        }

        std::cout << "  ✓ " << absent_tested << " absent key lookups returned nullopt\n";
    }
    
    // --- Test 6: get_ref API for large objects ---
    {
        std::cout << "[Test 6] get_ref API (avoid copying)...\n";
        
        // Test with a large object
        struct LargeObject {
            std::vector<int> data;
            std::string description;
            
            LargeObject() = default;
            LargeObject(int size, std::string desc) 
                : data(size, 42), description(std::move(desc)) {}
        };
        
        MPQ::HashTable<LargeObject> table;
        
        // Insert large objects
        table.put("obj1", LargeObject(1000, "First object"));
        table.put("obj2", LargeObject(2000, "Second object"));
        
        // Test const get_ref
        const auto& const_table = table;
        const LargeObject* ptr1 = const_table.get_ref("obj1");
        assert(ptr1 != nullptr);
        assert(ptr1->data.size() == 1000);
        assert(ptr1->description == "First object");
        
        // Test nullptr for non-existent key
        const LargeObject* ptr_none = const_table.get_ref("nonexistent");
        assert(ptr_none == nullptr);
        
        // Test mutable get_ref
        LargeObject* ptr2 = table.get_ref("obj2");
        assert(ptr2 != nullptr);
        ptr2->description = "Modified";
        
        // Verify modification persisted
        const LargeObject* ptr2_check = const_table.get_ref("obj2");
        assert(ptr2_check->description == "Modified");
        
        // Test that get() still works (but copies)
        auto copied = table.get("obj1");
        assert(copied.has_value());
        assert(copied->data.size() == 1000);
        
        std::cout << "  ✓ get_ref() avoids copying large objects\n";
        std::cout << "  ✓ Mutable get_ref() allows in-place modification\n";
        std::cout << "  ✓ get() still works for backward compatibility\n";
    }
    
    // --- Test 7: for_each iteration ---
    {
        std::cout << "[Test 7] for_each iteration...\n";
        MPQ::HashTable<int> table;
        
        for (int i = 0; i < 10; ++i) {
            table.put("key" + std::to_string(i), i * 10);
        }
        
        int sum = 0;
        int count = 0;
        table.for_each([&](std::string_view /* key */, int value) {
            sum += value;
            ++count;
        });
        
        assert(count == 10);
        assert(sum == 450);  // 0+10+20+...+90
        
        std::cout << "  ✓ Iterated over all " << count << " entries\n";
    }

    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
