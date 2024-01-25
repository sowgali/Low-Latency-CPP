#pragma once

#include <sstream>
#include "common/types.hpp"
#include "common/lf_queue.hpp"

using namespace Common;

namespace Exchange {

    #pragma pack(push, 1)
    
    enum class MarketUpdateType : uint8_t {
        INVALID=0,
        ADD=1,
        MODIFY=2,
        CANCEL=3,
        TRADE=4
    };

    inline auto MarketUpdateTypeToString(MarketUpdateType marketUpdateType) -> std::string {
        switch (marketUpdateType){
            case MarketUpdateType::INVALID:
                return "INVALID";
            case MarketUpdateType::ADD:
                return "ADD";
            case MarketUpdateType::MODIFY:
                return "MODIFY";
            case MarketUpdateType::CANCEL:
                return "CANCEL";
            case MarketUpdateType::TRADE:
                return "TRADE";
        }
        return "UNKNOWN";
    }

    struct MEMarketUpdate {
        MarketUpdateType type_ = MarketUpdateType::INVALID;
        OrderId orderId_ = OrderId_INVALID;
        TickerId tickerId_ = TickerId_INVALID;
        ClientId clientId_ = ClientId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;
        Priority priority_ = Priority_INVALID;

        auto toString() const {
            std::stringstream ss;
            ss  << "MEMarketUpdateObject" << " ["
                << " type: " << MarketUpdateTypeToString(type_)
                << " client: " << ToString<ClientId>(clientId_)
                << " ticker: " << ToString<TickerId>(tickerId_)
                << " oid: " << ToString<OrderId>(orderId_)
                << " side: " << sideToString(side_)
                << " qty: " << ToString<Qty>(qty_)
                << " price: " << ToString<Price>(price_)
                << " priority: " << ToString<Priority>(priority_)
                << "]";
            ss.str();
        }
    };

    #pragma pack(pop)

    typedef LFQueue<MEMarketUpdate> MarketUpdateLFQueue;
}