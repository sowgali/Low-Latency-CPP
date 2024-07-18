#pragma once

#include <iostream>
#include <string>

inline auto ASSERT(bool cond, std::string& msg) noexcept {
    if(!cond) [[unlikely]] {
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline auto FATAL(const std::string& msg) noexcept {
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}