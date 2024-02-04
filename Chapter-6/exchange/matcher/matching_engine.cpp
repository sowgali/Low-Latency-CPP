#include "matching_engine.hpp"

namespace Exchange {
    MatchingEngine::MatchingEngine(OrderBookHashMap orderBookHashMap_, ClientRequestLFQueue* clientRequestLFQueue_, ClientResponseLFQueue* clientResponseLFQueue_, MarketUpdateLFQueue* marketQueueUpdate_) : 
        ticker_order_book_(orderBookHashMap_), incoming_requests_(clientRequestLFQueue_),
        outgoing_ogw_responses_(clientResponseLFQueue_), outgoing_responses_(marketQueueUpdate_),
        logger_("exchange_matching_engine.log"){
            for(size_t i = 0; i < ticker_order_book_.size(); i++){
                ticker_order_book_[i] = new MEOrderBook(i, this, &(this->logger_));
            }
        }
    
    MatchingEngine::~MatchingEngine(){
        run_ = false;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        incoming_requests_ = nullptr;
        outgoing_ogw_responses_ = nullptr;
        outgoing_responses_ = nullptr;
        for(auto& orderbook: ticker_order_book_){
            delete orderbook;
            orderbook = nullptr;
        }
    }

    auto MatchingEngine::start() -> void {
        run_ = true;
        ASSERT(Common::createAndStartThread(-1, "exchange/matching_engine", [this](){this->run();}) != nullptr, "Failed to start matching engine");
    }

    auto MatchingEngine::stop() -> void {
        run_ = false;
    }

    auto MatchingEngine::run() noexcept -> void {
        logger_.log("%:% %() %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_));
        while(run_){
            const auto me_client_request = incoming_requests_->getNextToRead();
            if(me_client_request)[[likely]]{
                logger_.log("%:% %() % Processing: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), me_client_request->toString());
                processClientRequest(me_client_request);
                incoming_requests_->updateReadIdx();
            }
        }
    }

    auto MatchingEngine::processClientRequest(const MEClientRequest* clientRequest_) noexcept -> void {
        auto orderbook = this->ticker_order_book_[clientRequest_->tickerId_];
        switch (clientRequest_->clientRequestType_){
            case ClientRequestType::NEW :
                orderbook->add(clientRequest_->clientId_, clientRequest_->orderId_, clientRequest_->tickerId_, clientRequest_->side_, clientRequest_->price_, clientRequest_->qty_);
                break;
            case ClientRequestType::CANCEL :
                orderbook->cancel(clientRequest_->clientId_, clientRequest_->orderId_, clientRequest_->tickerId_);
                break;
            default:
                FATAL("Received invalid client-request-type: " + clientRequestTypeToString(clientRequest_->clientRequestType_));
                break;
        }
    }

    auto MatchingEngine::sendClientResponse(const MEClientResponse* clientResponse_) noexcept -> void {
        logger_.log("%:% %() % Sending: %", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), clientResponse_->toString());
        auto next_write_loc = outgoing_ogw_responses_->getNextToWrite();
        *next_write_loc = std::move(*clientResponse_);
        outgoing_ogw_responses_->updateWriteIdx();
    }

    auto MatchingEngine::sendMarketUpdate(const MEMarketUpdate* marketUpdate_) noexcept -> void {
        logger_.log("%:% %() % Sending: %", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), marketUpdate_->toString());
        auto next_write_loc = outgoing_responses_->getNextToWrite();
        *next_write_loc = std::move(*marketUpdate_);
        outgoing_responses_->updateWriteIdx();
    }

}