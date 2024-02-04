#include <csignal>
#include "matcher/matching_engine.hpp"

Common::Logger* logger_ = nullptr;
Exchange::MatchingEngine* matching_engine_ = nullptr;

void signal_handler(int) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10s);
    delete logger_;
    logger_ = nullptr;
    delete matching_engine_;
    matching_engine_ = nullptr;
    std::this_thread::sleep_for(10s);
    exit(EXIT_SUCCESS);
}

int main(int,char**){
    logger_ = new Common::Logger("exchange_main.log");
    std::signal(SIGINT, signal_handler);
    const int sleep_time = 100 * 1000;
    Exchange::ClientRequestLFQueue clientRequestLFQueue_(ME_MAX_CLIENT_UPDATES);
    Exchange::ClientResponseLFQueue clientResponseLFQueue_(ME_MAX_CLIENT_UPDATES);
    Exchange::MarketUpdateLFQueue marketUpdateLFQueue_(ME_MAX_MARKET_UPDATES);

    std::string time_str_;
    logger_->log("%:% %() % Starting Matching Engine...\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_));

    while(true){
        logger_->log("%:% %() % Sleeping for a few milliseconds", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_));
        usleep(sleep_time * 1000);
    }
}