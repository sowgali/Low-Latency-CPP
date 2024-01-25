#pragma once

#include <sstream>
#include "common/types.hpp"
#include "common/lf_queue.hpp"

using namespace Common;

namespace Exchange {

    #pragma pack(push, 1)
    enum class ClientResponseType : uint8_t {
        INVALID=0,
        ACCEPTED=1,
        CANCELLED=2,
        FILLED=3,
        CANCEL_REJECTED=4
    };

    inline auto clientResponseToString(ClientResponseType clientResponseType) -> std::string {
        switch (clientResponseType){
            case ClientResponseType::INVALID:
                return "INVALID";
            case ClientResponseType::ACCEPTED:
                return "ACCEPTED";
            case ClientResponseType::CANCELLED:
                return "CANCELLED";
            case ClientResponseType::FILLED:
                return "FILLED";
            case ClientResponseType::CANCEL_REJECTED:
                return "CANCEL_REJECTED";
        }
        return "UNKNOWN";
    }

    struct MEClientResponse {
        ClientResponseType clientResponseType = ClientResponseType::INVALID;
        ClientId clientId_ = ClientId_INVALID;
        TickerId tickerId_ = TickerId_INVALID;
        OrderId clientOrderId_ = OrderId_INVALID;
        OrderId marketOrderId_ = OrderId_INVALID;
        Side side_ = Side::INVALID;
        Qty executedQty_ = Qty_INVALID;
        Qty leftOverQty_ = Qty_INVALID;

        auto toString() const {
            std::stringstream ss;
            ss  << "MEClientResponseObject" << " ["
                << "type: " << clientResponseToString(clientResponseType)
                << " client: " << ToString<ClientId>(clientId_)
                << " ticker: " << ToString<TickerId>(tickerId_)
                << " coid: " << ToString<OrderId>(clientOrderId_)
                << " moid: " << ToString<OrderId>(marketOrderId_)
                << " side: " << sideToString(side_)
                << " exec_qty: " << ToString<Qty>(executedQty_)
                << " leave_qty_: " << ToString<Qty>(leftOverQty_)
                << "]";
            return ss.str();
        }

    };

    #pragma pack(pop)

    typedef LFQueue<MEClientResponse> ClientResponseLFQueue;
}