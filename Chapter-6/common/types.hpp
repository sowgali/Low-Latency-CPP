#pragma once

#include <cstdint>
#include <limits>
#include "macros.hpp"

namespace Common {
    typedef uint64_t OrderId;
    constexpr auto OrderId_INVALID = std::numeric_limits<OrderId>::max();

    typedef uint32_t TickerId;
    constexpr auto TickerId_INVALID = std::numeric_limits<TickerId>::max();
  
    typedef uint32_t ClientId;
    constexpr auto ClientId_INVALID = std::numeric_limits<ClientId>::max();

    typedef uint64_t Price;
    constexpr auto Price_INVALID = std::numeric_limits<Price>::max();

    typedef uint32_t Qty;
    constexpr auto Qty_INVALID = std::numeric_limits<Qty>::max();

    typedef uint64_t Priority;
    constexpr auto Priority_INVALID = std::numeric_limits<Priority>::max();

    template<typename T>
    inline auto ToString(T arg) -> std::string {
        if(arg == std::numeric_limits<T>::max()) [[unlikely]] {
            return "INVALID";
        }
        return std::to_string(arg);
    }

    enum Side : int8_t {
        INVALID = 0,
        BUY = 1,
        SELL = -1
    };

    inline auto sideToString(Side side) -> std::string {
        switch (side){
            case Side::BUY:
                return "BUY";
            case Side::SELL:
                return "SELL";
            case Side::INVALID:
                return "INVALID";
        }
        return "UNKNOWN";
    }

    constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;
    constexpr size_t ME_MAX_TICKERS = 8;
    constexpr size_t ME_MAX_CLIENT_UPDATES = 256 * 1024;
    constexpr size_t ME_MAX_MARKET_UPDATES = 256 * 1024;
    constexpr size_t ME_MAX_NUM_CLIENTS = 256;
    constexpr size_t ME_MAX_ORDER_IDS = 1024 * 1024;
    constexpr size_t ME_MAX_PRICE_LEVELS = 256;
}