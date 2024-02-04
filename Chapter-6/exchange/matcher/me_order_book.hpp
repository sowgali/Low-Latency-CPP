#pragma once

#include "matcher/me_order.hpp"
#include "matcher/matching_engine.hpp"
#include <array>

namespace Exchange {
    class MatchingEngine;
    class MEOrderBook final {

        public:
            MEOrderBook() = delete;
            MEOrderBook(MEOrderBook&) = delete;
            MEOrderBook(MEOrderBook&&) = delete;
            MEOrderBook& operator=(MEOrderBook&) = delete;
            MEOrderBook& operator=(MEOrderBook&&) = delete;

            MEOrderBook(TickerId tickerId_, MatchingEngine* matching_engine_, Logger* logger_):
                ticker_id_(tickerId_),
                matching_engine_(matching_engine_),
                logger_(logger_){}
            
            auto add(ClientId, OrderId, TickerId, Side, Price, Qty) noexcept -> void{};
            auto cancel(ClientId, OrderId, TickerId) noexcept -> void{};

        private:
            TickerId ticker_id_ = TickerId_INVALID;
            MatchingEngine* matching_engine_ = nullptr;
            Logger* logger_ = nullptr;
    };

    typedef std::array<MEOrderBook*, ME_MAX_TICKERS> OrderBookHashMap;
}