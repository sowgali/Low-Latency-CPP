#include "tcp_server.hpp"

namespace Common {
    auto TCPServer::addToEpollList(Common::TCPSocket* s) {
        epoll_event ev{EPOLLET | EPOLLIN, {reinterpret_cast<void*>(s)}};
        return !epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, s->getFD(), &ev);
    }

    auto TCPServer::listen(const std::string& iface, int port) noexcept -> void {
        epoll_fd_ = epoll_create(1);
        ASSERT(epoll_fd_ >= 0, "epoll_create() failed. Error: " + std::string(strerror(errno)));
        ASSERT(listener_socket_.connect("", iface, port, true) >= 0, "listener_socket_ failed to connect, iface: " + iface + ", port: " + std::to_string(port) + " error: " + std::string(strerror(errno)));
        ASSERT(addToEpollList(&listener_socket_), "addtoEpollList() failed, Error: " + std::string(strerror(errno)));
    }

    auto TCPServer::sendAndRecv() noexcept -> void {
        auto recv = false;
        std::for_each(receive_sockets_.begin(), receive_sockets_.end(), [&recv](auto socket){
            recv |= socket->sendAndRecv();
        });
        if(recv)
            recv_finished_callback_();
        std::for_each(send_sockets_.begin(), send_sockets_.end(), [](auto socket){
            socket->sendAndRecv();
        });
    }

    auto TCPServer::poll() noexcept -> void {
        const int maxEvents = 1 + receive_sockets_.size() + send_sockets_.size();
        const int n = epoll_wait(epoll_fd_, events_, maxEvents, 0);
        bool have_new_connection = false;
        for(int i = 0; i < n; i++){
            const auto& event = events_[i];
            auto socket = reinterpret_cast<TCPSocket*>(event.data.ptr);
            if(event.events & EPOLLIN){
                if(socket == &listener_socket_){
                    logger_.log("% : % %() % EPOLLIN listener_socket: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), socket->getFD());
                    have_new_connection = true;
                    continue;
                }
                logger_.log("% : % %() % EPOLLIN socket: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), socket->getFD());
                if(std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) == receive_sockets_.end()){
                    receive_sockets_.push_back(socket);
                }
            }

            if(event.events & EPOLLOUT){
                logger_.log("% : % %() % EPOLLOUT socket: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), socket->getFD());
                if(std::find(send_sockets_.begin(), send_sockets_.end(), socket) == send_sockets_.end()){
                    send_sockets_.push_back(socket);
                }
            }

            if(event.events & (EPOLLERR | EPOLLHUP)){
                logger_.log("% : % %() % EPOLLERR socket: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), socket->getFD());
                if(std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) == receive_sockets_.end()){
                    receive_sockets_.push_back(socket);
                }
            }
        }

        while(have_new_connection){
            logger_.log("% : % %() % have_new_connection\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_));
            sockaddr_storage addr;
            socklen_t addr_len = sizeof(addr);
            int fd = accept(listener_socket_.getFD(), reinterpret_cast<sockaddr*>(&addr), &addr_len);
            if(fd == -1)
                break;
            ASSERT(setNonBlocking(fd) && setNoDelay(fd), "Failed to set non-blocking or no-delay on socket: " + std::to_string(fd));
            logger_.log("% : % %() % accepted socket:%\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str_), fd);
            auto socket = new TCPSocket(logger_, recv_callback_);
            socket->setFD(fd);
            ASSERT(addToEpollList(socket), "Unable to add socket. error: " + std::string(strerror(errno)));
            if(std::find(receive_sockets_.begin(), receive_sockets_.end(), socket) == receive_sockets_.end()){
                receive_sockets_.push_back(socket);
            }
        }
    }
}