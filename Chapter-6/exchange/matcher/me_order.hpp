#pragma once

#include "common/types.hpp"
#include <sstream>
#include <array>

using namespace Common;

namespace Exchange {
    struct MEOrder{
        TickerId tickerId_ = TickerId_INVALID;
        ClientId clientId_ = ClientId_INVALID;
        OrderId clientOrderId_ = OrderId_INVALID;
        OrderId marketOrderId_ = OrderId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;
        Priority priority_ = Priority_INVALID;
        MEOrder* prev_order_ = nullptr;
        MEOrder* next_order_ = nullptr;

        MEOrder() = default;

        MEOrder(TickerId tickerId_, ClientId clientId_, OrderId clientOrderId_, OrderId marketOrderId_, Side side_, Price price_, Qty qty_, Priority priority_, MEOrder* prev_order_, MEOrder* next_order_) :
            tickerId_(tickerId_),
            clientId_(clientId_),
            clientOrderId_(clientOrderId_),
            marketOrderId_(marketOrderId_),
            side_(side_),
            price_(price_),
            qty_(qty_),
            priority_(priority_),
            prev_order_(prev_order_),
            next_order_(next_order_) {}
        
        auto toString() const -> std::string;
    };

    typedef std::array<MEOrder*, ME_MAX_ORDER_IDS> OrderHashMap;
    typedef std::array<OrderHashMap, ME_MAX_NUM_CLIENTS> ClientOrderHashMap;

    struct MEOrdersAtPrice {
        Price price_ = Price_INVALID;
        Side side_ = Side::INVALID;
        MEOrder* first_order_ = nullptr;
        MEOrdersAtPrice* prev_entry_ = nullptr;
        MEOrdersAtPrice* next_entry_ = nullptr;

        MEOrdersAtPrice() = default;
        MEOrdersAtPrice(Price price_, Side side_, MEOrder* first_order_, MEOrdersAtPrice* prev_entry_, MEOrdersAtPrice* next_entry_) : 
            price_(price_), side_(side_), first_order_(first_order_), prev_entry_(prev_entry_), next_entry_(next_entry_) {}
        
        auto toString() const -> std::string {
            std::stringstream ss;
            ss << "MEOrderAtPrice: ["
            << "side: " << sideToString(side_) << " "
            << "price: " << ToString<Price>(price_) << " "
            << "first_order: " << (first_order_ ? first_order_->toString() : "null") << " "
            << "prev_entry: " << ToString<Price>(prev_entry_ ? prev_entry_->price_ : Price_INVALID) << " "
            << "next_entry: " << ToString<Price>(next_entry_ ? next_entry_->price_ : Price_INVALID) << "]";
            return ss.str();
        }
    };

    typedef std::array<MEOrdersAtPrice*, ME_MAX_PRICE_LEVELS> OrdersAtPriceHashMap;
}