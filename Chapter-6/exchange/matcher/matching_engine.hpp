#pragma once

#include "common/lf_queue.hpp"
#include "common/logging.hpp"
#include "common/thread_utils.hpp"

#include "order_server/client_request.hpp"
#include "order_server/client_response.hpp"
#include "market_data/market_update.hpp"

#include "matcher/me_order_book.hpp"

using namespace Common;

namespace Exchange {
    class MatchingEngine final {
        public:
            
            MatchingEngine(OrderBookHashMap, ClientRequestLFQueue*, ClientResponseLFQueue*, MarketUpdateLFQueue*);
            
            ~MatchingEngine();

            MatchingEngine() = delete;
            MatchingEngine(const MatchingEngine&) = delete;
            MatchingEngine(const MatchingEngine&&) = delete;
            MatchingEngine& operator=(const MatchingEngine&) = delete;
            MatchingEngine& operator=(const MatchingEngine&&) = delete;

            auto start() -> void;
            auto stop() -> void;
            auto run() noexcept -> void;
            auto processClientRequest(const MEClientRequest*) noexcept -> void;
            auto sendClientResponse(const MEClientResponse*) noexcept -> void;
            auto sendMarketUpdate(const MEMarketUpdate*) noexcept -> void;

        private:
            OrderBookHashMap ticker_order_book_;
            ClientRequestLFQueue* incoming_requests_ = nullptr;
            ClientResponseLFQueue* outgoing_ogw_responses_ = nullptr;
            MarketUpdateLFQueue* outgoing_responses_ = nullptr;
            volatile bool run_ = false;
            Logger logger_;
            std::string time_str_;
    };
}