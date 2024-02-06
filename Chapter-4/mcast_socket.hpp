#pragma once

#include "socket_utils.hpp"
#include "logging.hpp"
#include <functional>

namespace Common {
    const size_t mcastBufferSize = 64*64*1024;
    class mcastSocket {
        public:
            mcastSocket(Logger& logger) : logger_(logger) {
                outbound_data_.resize(mcastBufferSize);
                inbound_data_.resize(mcastBufferSize);
            }
        private:
            int sock_fd_ = -1;
            Logger& logger_;
            std::vector<char> outbound_data_;
            size_t next_valid_send_idx_ = 0;
            std::vector<char> inbound_data_;
            size_t next_valid_recv_idx_ = 0;
            std::string time_str_;
            std::function<void(mcastSocket* s)> recv_call_back_ = nullptr;
    };
}