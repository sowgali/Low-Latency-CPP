#pragma once

// #define LIKELY(x) __builtin_expect(!!(x), 1)
// #define UNLIKELY(x) __builtin_expect(!!(x), 0)

#include <iostream>
#include <string>

inline auto ASSERT(const bool cond, const std::string& msg) noexcept {
    if(!cond)[[unlikely]]{ 
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline auto FATAL(const std::string& msg) noexcept {
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}