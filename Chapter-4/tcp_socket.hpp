#pragma once

#include "socket_utils.hpp"
#include "logging.hpp"
#include "time_utils.hpp"
#include <functional>

namespace Common {
    const size_t tcpBufferSize = 64 * 1024 * 1024;

    class TCPSocket {
        public:
            TCPSocket() = delete;
            TCPSocket(const TCPSocket&) = delete;
            TCPSocket(const TCPSocket&&) = delete;
            TCPSocket& operator=(const TCPSocket&) = delete;
            TCPSocket& operator=(const TCPSocket&&) = delete;

            explicit TCPSocket(Logger& logger) : logger_(logger) {
                send_buffer_ = new char[tcpBufferSize];
                recv_buffer_ = new char[tcpBufferSize];
                recv_callback_ = [this] (auto tcpSocket, auto rx_time){defaultRecvCallBack(tcpSocket, rx_time);};
            }

            explicit TCPSocket(Logger& logger, auto recvCallback) : logger_(logger), recv_callback_(recv_callback_) {
                send_buffer_ = new char[tcpBufferSize];
                recv_buffer_ = new char[tcpBufferSize];
            }

            auto getFD() noexcept {
                return fd_;
            }

            auto getNextRecvValidIdx() noexcept {
                return next_recv_valid_idx_;
            }

            ~TCPSocket(){
                destroy();
                delete[] send_buffer_;
                send_buffer_ = nullptr;
                next_send_valid_idx_ = 0;
                delete[] recv_buffer_;
                recv_buffer_ = nullptr;
                next_recv_valid_idx_ = 0;
            }

        private:
            Logger& logger_;
            int fd_ = -1;
            char* send_buffer_ = nullptr;
            char* recv_buffer_ = nullptr;
            bool send_disconnected_ = false;
            bool recv_disconnected_ = false;
            size_t next_send_valid_idx_ = 0;
            size_t next_recv_valid_idx_ = 0;
            std::function<void(TCPSocket* tcpSocket, Nanos rx_time)> recv_callback_ = nullptr;
            struct sockaddr_in sock_attrib_;
            std::string time_str_;

            auto defaultRecvCallBack(TCPSocket* tcpSocket, Nanos rx_time) noexcept {
                logger_.log("% : % %() % TCPSocket::defaultRecvCallBack() socket: % len: % rx: %\n", __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_), tcpSocket->getFD(), tcpSocket->getNextRecvValidIdx(), rx_time);
            }

            auto destroy() noexcept -> void;
            
            auto connect(const std::string& ip, const std::string& iface, int port, bool is_listening) noexcept -> int;

            auto sendAndRecv() noexcept -> bool;

            auto send(const char* sendData, size_t len) noexcept -> void;
    };
}