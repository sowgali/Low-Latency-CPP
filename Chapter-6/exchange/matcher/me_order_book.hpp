#pragma once

#include "matcher/me_order.hpp"
#include "matcher/matching_engine.hpp"
#include "common/mem_pool.hpp"
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

            MEOrderBook(TickerId, MatchingEngine*, Logger*);
            
            auto add(ClientId, OrderId, TickerId, Side, Price, Qty) noexcept -> void;
            auto cancel(ClientId, OrderId, TickerId) noexcept -> void;

        private:
            TickerId ticker_id_ = TickerId_INVALID;
            MatchingEngine* matching_engine_ = nullptr;
            Logger* logger_ = nullptr;
            ClientOrderHashMap cid_oid_to_order_;
            MemPool<MEOrdersAtPrice> orders_at_price_pool_;
            MEOrdersAtPrice* bids_by_price_ = nullptr;
            MEOrdersAtPrice* asks_by_price_ = nullptr;
            OrdersAtPriceHashMap price_orders_at_price_;
            MemPool<MEOrder> order_pool_;
            OrderId next_order_id_ = 1;
            MEClientResponse client_response_;
            MEMarketUpdate market_update_;
            std::string time_str_;

            auto toString(bool detailed, bool validity_check) -> std::string;

            auto generateNewMarketOrderId() noexcept -> OrderId {
                return next_order_id_++;
            }

            auto priceToIndex(Price price) const noexcept {
                return (price % ME_MAX_PRICE_LEVELS);
            }

            auto getOrdersAtPrice(Price price) const noexcept -> MEOrdersAtPrice* {
                return price_orders_at_price_.at(priceToIndex(price));
            }

            auto checkMatch(ClientId, OrderId, TickerId, Side, Price, Qty, OrderId) noexcept -> Qty;

            auto addOrder(MEOrder*) noexcept -> void;

            auto getNextPriority(Price) noexcept -> Priority;

            auto addOrderAtPrice(MEOrdersAtPrice* order_at_price_) noexcept -> void;
    };

    typedef std::array<MEOrderBook*, ME_MAX_TICKERS> OrderBookHashMap;
}