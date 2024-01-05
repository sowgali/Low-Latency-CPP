#pragma once
#include "tcp_socket.hpp"
#include <sys/epoll.h>

namespace Common {
    class TCPServer {
        private:
            auto addToEpollList(TCPSocket* s);
        public:
            int epoll_fd_ = -1;
            TCPSocket listener_socket_;
            epoll_event events_[1024];
            std::vector<TCPSocket*> receive_sockets_, send_sockets_;
            std::function<void(TCPSocket* s, Nanos rx_time)> recv_callback_ = nullptr;
            std::function<void()> recv_finished_callback_ = nullptr;
            std::string time_str_;
            Logger& logger_;

            explicit TCPServer(Logger& logger_) : listener_socket_(logger_), logger_(logger_){}

            auto listen(const std::string& iface, int port) noexcept -> void;

            auto poll() noexcept -> void;

            auto sendAndRecv() noexcept -> void;

            TCPServer() = delete;
            TCPServer(TCPServer& tcpServer) = delete;
            TCPServer(TCPServer&& tcpServer) = delete;
            TCPServer& operator=(TCPServer& tcpServer) = delete;
            TCPServer& operator=(TCPServer&& tcpServer) = delete;
    };
}