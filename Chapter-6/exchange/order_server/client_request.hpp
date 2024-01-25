#pragma once

#include <sstream>
#include "common/types.hpp"
#include "common/lf_queue.hpp"

using namespace Common;

namespace exchange{
    #pragma pack(push, 1)

    enum class ClientRequestType : uint8_t {
        INVALID=0,
        NEW=1,
        CANCEL=2
    };

    inline auto clientRequestTypeToString(ClientRequestType clientRequestTypeToString) -> std::string {
        switch (clientRequestTypeToString){
            case ClientRequestType::INVALID:
                return "INVALID";
            case ClientRequestType::CANCEL:
                return "CANCEL";
            case ClientRequestType::NEW:
                return "NEW";
        }
        return "UNKNOWN";
    }

    struct MEClientRequest {
        ClientRequestType clientRequestType_ = ClientRequestType::INVALID;
        ClientId clientId_ = ClientId_INVALID;
        OrderId orderId_ = OrderId_INVALID;
        TickerId tickerId_ = TickerId_INVALID;
        Side side_ = Side::INVALID;
        Price price_ = Price_INVALID;
        Qty qty_ = Qty_INVALID;

        auto toString() const {
            std::stringstream ss;
            ss  << "MEClientRequestObject" << " ["
                << "type: " << clientRequestTypeToString(clientRequestType_)
                << " client: " << ToString<ClientId>(clientId_)
                << " ticker: " << ToString<TickerId>(tickerId_)
                << " oid: " << ToString<OrderId>(orderId_)
                << " side: " << sideToString(side_)
                << " qty: " << ToString<Qty>(qty_)
                << " price: " << ToString<Price>(price_)
                << "]";
            return ss.str();
        }
    };

    #pragma pack(pop)

    typedef LFQueue<MEClientRequest> ClientRequestLFQueue;
}
