#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <string>
#include <tuple>
#include <format>
#include <typeinfo>

// Demangle a GCC/Clang RTTI name at runtime.
// Throws std::runtime_error on failure.
std::string demangle(const char* mangled) {
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> ptr{
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
        std::free
    };
    if (status != 0 || !ptr) {
        throw std::runtime_error(std::format("demangle failed (status = {})", status));
    }
    return ptr.get();
}

// Get a human‐readable name for type T.
template<typename T>
std::string type_name() {
    return demangle(typeid(T).name());
}

// Get the exact type of an expression (including references and cv‐qualifiers).
template<typename U>
std::string type_name_of(U&&) {
    return demangle(typeid(U).name());
}

struct S {};

int main() {
    // CTAD for std::tuple in C++20
    auto tup = std::tuple{ 2,
                           "3",
                           "abcd",
                           std::string{"4"},
                           S{} };

    // Structured binding
    const auto& [i, c1, c2, str, s_obj] = tup;

    // Print with std::format (C++20)
    std::cout << std::format("{:<8} : {}\n", "i",           type_name<decltype(i)>());
    std::cout << std::format("{:<8} : {}\n", "c1",          type_name_of(c1));
    std::cout << std::format("{:<8} : {}\n", "c2",          type_name_of(c2));
    std::cout << std::format("{:<8} : {}\n", "str",         type_name<decltype(str)>());
    std::cout << std::format("{:<8} : {}\n", "s_obj",       type_name<decltype(s_obj)>());

    return 0;
}
