#include "me_order.hpp"
#include <sstream>

namespace Exchange {
    auto MEOrder::toString() const -> std::string{
        std::stringstream ss;
        ss << "MEOrder: [" 
        << "ticker: " << ToString<TickerId>(tickerId_) << " "
        << "cid: " << ToString<ClientId>(clientId_) << " "
        << "oid: " << ToString<OrderId>(marketOrderId_) << " "
        << "moid: " << ToString<OrderId>(clientOrderId_) << " "
        << "side: " << sideToString(side_) << " "
        << "price: " << ToString<Price>(price_) << " "
        << "qty: " << ToString<Qty>(qty_) << " "
        << "prio: " << ToString<Priority>(priority_) << " "
        << "prev: " << ToString<OrderId>(prev_order_ ? prev_order_->marketOrderId_ : OrderId_INVALID) << " "
        << "next: " << ToString<OrderId>(next_order_ ? next_order_->marketOrderId_ : OrderId_INVALID)
        << "]";
        return ss.str();
    }
}