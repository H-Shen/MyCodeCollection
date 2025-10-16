// main.cpp
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>
#include <format>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <optional>
#include <variant>
#include <functional>
#include <array>
#include <typeinfo>
#include <tuple>
#include <utility>

// ============================================================================
// Runtime Type Name (using RTTI + demangling)
// - Works at runtime
// - Loses cv-qualifiers and references
// - Good for polymorphic types
// ============================================================================

[[nodiscard]] std::string demangle(const char* mangled) {
    int status = 0;
    const std::unique_ptr<char, void(*)(void*)> holder{
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
        std::free
    };
    if (status != 0 || !holder) {
#if __cpp_lib_format
        throw std::runtime_error(std::format("demangle failed (status = {})", status));
#else
        throw std::runtime_error("demangle failed");
#endif
    }
    return std::string{holder.get()};
}

template<typename T>
[[nodiscard]] std::string type_name_runtime() {
    return demangle(typeid(T).name());
}

// ============================================================================
// Compile-Time Type Name (using compiler intrinsics)
// - Works at compile time (constexpr, can be evaluated at compile-time)
// - Preserves cv-qualifiers, references, and value categories
// - Returns std::string_view (zero overhead)
// ============================================================================

template <typename T>
[[nodiscard]] constexpr std::string_view type_name_full() {
    constexpr std::string_view pf = __PRETTY_FUNCTION__;

    // 1) Common starting patterns for GCC/Clang
    constexpr std::string_view starts[] = {
        " [with T = ",   // GCC typical
        " [T = ",        // Clang / minor variants
        "[with T = ",
        "[T = "
    };

    std::size_t start = std::string_view::npos;
    for (auto m : starts) {
        const auto pos = pf.find(m);
        if (pos != std::string_view::npos) {
            start = pos + m.size();
            break;
        }
    }

    // 2) Fallback: locate '[' -> 'T' -> '=' -> skip spaces
    if (start == std::string_view::npos) {
        constexpr auto lb = pf.find('[');
        if constexpr (lb != std::string_view::npos) {
            if (auto t = pf.find('T', lb); t != std::string_view::npos) {
                auto eq = pf.find('=', t);
                if (eq != std::string_view::npos) {
                    start = eq + 1;
                    while (start < pf.size() && pf[start] == ' ') ++start;
                }
            }
        }
    }

    // 3) End position: prefer ';' (GCC adds aliases) else ']'
    std::size_t end = std::string_view::npos;
    if (start != std::string_view::npos) {
        const auto semi = pf.find(';', start);
        if (const auto rbr  = pf.find(']', start); semi != std::string_view::npos && (rbr == std::string_view::npos || semi < rbr)) {
            end = semi;
        } else {
            end = rbr;
        }
    }

    // 4) Return substring if successful, otherwise the full function name
    return (start != std::string_view::npos && end != std::string_view::npos && end > start)
         ? pf.substr(start, end - start)
         : pf;
}



// Get the exact type of an expression (preserves everything)
// Uses perfect forwarding to deduce the exact type including value category
template<typename T>
[[nodiscard]] constexpr std::string_view type_of(T&&) {
    return type_name_full<T>();
}

// ============================================================================
// Print Utilities (using std::format + std::cout for GCC 13 compatibility)
// - Avoid overload ambiguity by constraining format overloads to args>0
// ============================================================================

template<typename... Args>
    requires (sizeof...(Args) > 0)
void my_println(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template<typename... Args>
    requires (sizeof...(Args) > 0)
void my_print(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...);
}

inline void my_println(std::string_view s) {
    std::cout << s << '\n';
}

// ============================================================================
// Test Utilities
// ============================================================================

struct CustomType {
    int x;
    double y;
};

// Print type information with label
template<typename T>
void print_type(std::string_view label) {
    my_println("{:<35} : {}", label, type_name_full<T>());
}

// Print type information for an expression
template<typename T>
void print_type_of(std::string_view label, T&& expr) {
    my_println("{:<35} : {}", label, type_of(std::forward<T>(expr)));
}

// ============================================================================
// Comprehensive Type Tests
// ============================================================================

void test_fundamental_types() {
    my_println("\n=== Fundamental Types ===");

    print_type<int>("int");
    print_type<const int>("const int");
    print_type<volatile int>("volatile int");
    print_type<const volatile int>("const volatile int");

    print_type<int&>("int&");
    print_type<const int&>("const int&");
    print_type<int&&>("int&&");
    print_type<const int&&>("const int&&");

    print_type<int*>("int*");
    print_type<const int*>("const int*");
    print_type<int* const>("int* const");
    print_type<const int* const>("const int* const");
}

void test_arrays() {
    my_println("\n=== Arrays ===");

    print_type<int[5]>("int[5]");
    print_type<const int[5]>("const int[5]");
    print_type<int[3][4]>("int[3][4]");
    print_type<int(&)[5]>("int(&)[5]");
    print_type<const int(&)[5]>("const int(&)[5]");
}

void test_containers() {
    my_println("\n=== STL Containers ===");

    // Sequential containers
    print_type<std::vector<int>>("vector<int>");
    print_type<const std::vector<int>>("const vector<int>");
    print_type<std::vector<int>&>("vector<int>&");
    print_type<const std::vector<int>&>("const vector<int>&");
    print_type<std::vector<int>&&>("vector<int>&&");

    print_type<std::array<double, 10>>("array<double, 10>");
    print_type<std::vector<std::vector<int>>>("vector<vector<int>>");

    // Container adapters
    print_type<std::stack<double>>("stack<double>");
    print_type<std::queue<char>>("queue<char>");
    print_type<std::priority_queue<int>>("priority_queue<int>");

    // Associative containers
    print_type<std::map<int, std::string>>("map<int, string>");
    print_type<std::set<double>>("set<double>");
    print_type<std::unordered_map<std::string, int>>("unordered_map<string, int>");
}

void test_smart_pointers() {
    my_println("\n=== Smart Pointers ===");

    print_type<std::unique_ptr<int>>("unique_ptr<int>");
    print_type<const std::unique_ptr<int>>("const unique_ptr<int>");
    print_type<std::unique_ptr<int>&>("unique_ptr<int>&");
    print_type<std::unique_ptr<int>&&>("unique_ptr<int>&&");

    print_type<std::shared_ptr<CustomType>>("shared_ptr<CustomType>");
    print_type<std::weak_ptr<std::string>>("weak_ptr<string>");
}

void test_modern_cpp_types() {
    my_println("\n=== Modern C++ Types ===");

    print_type<std::optional<int>>("optional<int>");
    print_type<const std::optional<int>&>("const optional<int>&");

    print_type<std::variant<int, double, std::string>>("variant<int, double, string>");

    print_type<std::function<int(double, std::string)>>("function<int(double, string)>");

    print_type<std::tuple<int, std::string, double>>("tuple<int, string, double>");
    print_type<std::pair<int, std::string>>("pair<int, string>");
}

void test_complex_nested_types() {
    my_println("\n=== Complex Nested Types ===");

    using ComplexType1 = std::map<std::string, std::vector<std::optional<CustomType>>>;
    print_type<ComplexType1>("map<string, vector<optional<CustomType>>>");
    print_type<const ComplexType1&>("const [above]&");

    using ComplexType2 = std::vector<std::unique_ptr<std::map<int, std::shared_ptr<std::string>>>>;
    print_type<ComplexType2>("vector<unique_ptr<map<int, shared_ptr<string>>>>");
}

void test_expressions() {
    my_println("\n=== Expression Type Deduction ===");

    int x = 42;
    const int cx = 42;
    int& rx = x;
    const int& crx = cx;

    print_type_of("x", x);
    print_type_of("(x)", (x));           // extra parentheses keep it an lvalue expression
    print_type_of("cx", cx);
    print_type_of("rx", rx);
    print_type_of("crx", crx);
    print_type_of("std::move(x)", std::move(x));  // rvalue reference
    print_type_of("42", 42);            // prvalue
    print_type_of("x + 1", x + 1);      // prvalue

    std::vector<int> vec{1, 2, 3};
    print_type_of("vec", vec);
    print_type_of("(vec)", (vec));
    print_type_of("vec[0]", vec[0]);
    print_type_of("std::move(vec)", std::move(vec));
}

void test_structured_bindings() {
    my_println("\n=== Structured Bindings ===");

    auto tup = std::tuple{42, std::string{"hello"}, 3.14, CustomType{1, 2.0}};

    // Structured bindings:
    // - `auto [x,...]` copies elements (new variables).
    // - `auto& / const auto& / auto&&` bind by reference (aliases).
    auto [i, str, d, obj] = tup;                 // copies
    print_type_of("i", i);
    print_type_of("str", str);
    print_type_of("d", d);
    print_type_of("obj", obj);

    const auto& [ci, cstr, cd, cobj] = tup;      // const references (aliases)
    print_type_of("ci", ci);
    print_type_of("cstr", cstr);
    print_type_of("cd", cd);
    print_type_of("cobj", cobj);

    auto&& [ri, rstr, rd, robj] = std::move(tup); // forwarding refs (aliases to moved-from tup)
    print_type_of("ri", ri);
    print_type_of("rstr", rstr);
    print_type_of("rd", rd);
    print_type_of("robj", robj);
}

void test_function_types() {
    my_println("\n=== Function Types ===");

    print_type<void()>("void()");
    print_type<int(double, std::string)>("int(double, string)");
    print_type<void(*)()>("void(*)()");
    print_type<int(*)(double, std::string)>("int(*)(double, string)");

    // Function reference
    print_type<void(&)()>("void(&)()");

    // Member function pointer (declarations only; class does not need such members to exist)
    print_type<int(CustomType::*)()>("int(CustomType::*)()");
    print_type<int(CustomType::*)() const>("int(CustomType::*)() const");
}

void test_comparison_runtime_vs_compile_time() {
    my_println("\n=== Runtime vs Compile-Time Comparison ===");

    [[maybe_unused]] const std::vector<int>& vec_ref = std::vector{1, 2, 3};

    my_println("{:<35} : {}", "Runtime (typeid)", type_name_runtime<decltype(vec_ref)>());
    my_println("{:<35} : {}", "Compile-time (intrinsic)", type_name_full<decltype(vec_ref)>());

    int&& rvalue_ref = 42;
    my_println("\n{:<35} : {}", "Runtime (typeid)", type_name_runtime<decltype(rvalue_ref)>());
    my_println("{:<35} : {}", "Compile-time (intrinsic)", type_name_full<decltype(rvalue_ref)>());
}

// ============================================================================
// Main Test Driver
// ============================================================================

int main() {
    my_println("╔══════════════════════════════════════════════════════════════════╗");
    my_println("║      C++23 Complete Type Inspector                               ║");
    my_println("║      Preserves cv-qualifiers, references, and value categories   ║");
    my_println("╚══════════════════════════════════════════════════════════════════╝");

    test_fundamental_types();
    test_arrays();
    test_containers();
    test_smart_pointers();
    test_modern_cpp_types();
    test_complex_nested_types();
    test_expressions();
    test_structured_bindings();
    test_function_types();
    test_comparison_runtime_vs_compile_time();

    my_println("\nAll type inspection tests completed successfully!");
    return 0;
}
