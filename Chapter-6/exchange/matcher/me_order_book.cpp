#include "matcher/me_order.hpp"
#include "matcher/matching_engine.hpp"

namespace Exchange {
    
    MEOrderBook::MEOrderBook(TickerId tickerId_, MatchingEngine* matching_engine_, Logger* logger_):
                ticker_id_(tickerId_),
                matching_engine_(matching_engine_),
                logger_(logger_),
                orders_at_price_pool_(ME_MAX_PRICE_LEVELS),
                order_pool_(ME_MAX_ORDER_IDS)
                {}
    
    MEOrderBook::~MEOrderBook(){
        logger_->log("%:% %() % Orderbook\n%\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), toString(false, true));
        matching_engine_ = nullptr;
        bids_by_price_ = nullptr;
        asks_by_price_ = nullptr;
        for(auto& it: cid_oid_to_order_){
            it.fill(nullptr);
        }
    }

    auto MEOrderBook::getNextPriority(Price price_) noexcept -> Priority {
        const auto order_at_price_ = getOrdersAtPrice(price_);
        if(order_at_price_ == nullptr){
            return 1lu;
        }
        return order_at_price_->first_order_->prev_order_->priority_+1;
    }

    auto MEOrderBook::add(ClientId client_id_, OrderId order_id_, TickerId ticket_id_, Side side_, Price price_, Qty qty_) noexcept -> void {
        const auto new_market_id_ = generateNewMarketOrderId();
        client_response_ = {ClientResponseType::ACCEPTED, client_id_, ticker_id_, order_id_, new_market_id_, side_, price_, 0, qty_};
        matching_engine_->sendClientResponse(&client_response_);
        const auto leavesQty = checkMatch(client_id_, order_id_, ticker_id_, side_, price_, qty_, new_market_id_);
        if(leavesQty > 0)[[likely]]{
            const auto priority_ = getNextPriority(price_);
            auto new_order_ = order_pool_.allocate(ticker_id_, client_id_, order_id_, new_market_id_, side_, price_, qty_, nullptr, nullptr);
            addOrder(new_order_);
            market_update_ = {MarketUpdateType::ADD, new_market_id_, ticker_id_, side_, price_, qty_, priority_};
            matching_engine_->sendMarketUpdate(&market_update_);
        }
    }

    auto MEOrderBook::addOrder(MEOrder* me_order_) noexcept -> void {
        const auto order_at_price_ = getOrdersAtPrice(me_order_->price_);
        if(order_at_price_ == nullptr) {
            me_order_->next_order_ = me_order_->prev_order_ = me_order_;
            auto new_order_at_price_ = orders_at_price_pool_.allocate(me_order_->price_, me_order_->side_, me_order_, nullptr, nullptr);
            addOrderAtPrice(new_order_at_price_);
        } else {
            auto first_order_ = order_at_price_->first_order_;
            first_order_->prev_order_->next_order_ = me_order_;
            me_order_->prev_order_ = first_order_->prev_order_;
            me_order_->next_order_ = first_order_;
            first_order_->prev_order_ = me_order_;
        }
        cid_oid_to_order_.at(me_order_->clientId_).at(me_order_->clientOrderId_) = me_order_;
    }

    auto MEOrderBook::addOrderAtPrice(MEOrdersAtPrice* me_order_price_) noexcept -> void {
        price_orders_at_price_.at(me_order_price_->price_) = me_order_price_;
        const auto best_orders_by_price_ = (me_order_price_->side_ == Side::BUY ? bids_by_price_ : asks_by_price_);
        if(best_orders_by_price_ == nullptr){
            me_order_price_->side_ == Side::BUY ? bids_by_price_ : asks_by_price_ = me_order_price_;
            me_order_price_->prev_entry_ = me_order_price_->next_entry_ = me_order_price_;
        } else {
            auto target = best_orders_by_price_;
            bool add_after = ((me_order_price_->side_ == Side::SELL && me_order_price_->price_ > target->price_) || (me_order_price_->side_ == Side::BUY && me_order_price_->price_ < target->price_));
            if(add_after){
                target = target->next_entry_;
                add_after = ((me_order_price_->side_ == Side::SELL && me_order_price_->price_ > target->price_) || (me_order_price_->side_ == Side::BUY && me_order_price_->price_ < target->price_));
            }
            while(add_after && target != best_orders_by_price_){
                add_after = ((me_order_price_->side_ == Side::SELL && me_order_price_->price_ > target->price_) || (me_order_price_->side_ == Side::BUY && me_order_price_->price_ < target->price_));
                if(add_after)
                    target = target->next_entry_;
            }

            if(add_after){
                if(target == best_orders_by_price_)
                    target = best_orders_by_price_->prev_entry_;
                me_order_price_->prev_entry_ = target->prev_entry_;
                me_order_price_->next_entry_ = target;
                target->prev_entry_->next_entry_ = me_order_price_;
                target->prev_entry_ = me_order_price_;
            } else {
                me_order_price_->prev_entry_ = target->prev_entry_;
                me_order_price_->next_entry_ = target;
                target->prev_entry_->next_entry_ = me_order_price_;
                target->prev_entry_ = me_order_price_;

                if((me_order_price_->side_ == Side::SELL && me_order_price_->price_ > target->price_) || (me_order_price_->side_ == Side::BUY && me_order_price_->price_ < target->price_)){
                    target->next_entry_ = (target->next_entry_ == best_orders_by_price_ ? me_order_price_ : target->next_entry_);
                    (me_order_price_->side_ == Side::BUY ? bids_by_price_ : asks_by_price_) = me_order_price_;
                }
            }
        }
    }
}