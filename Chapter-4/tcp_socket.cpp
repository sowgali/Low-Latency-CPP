#include "tcp_socket.hpp"

namespace Common {
    auto TCPSocket::destroy() noexcept -> void{
        close(fd_);
        fd_ = -1;
    }

    auto TCPSocket::connect(const std::string& ip, const std::string& iface, int port, bool is_listening) noexcept -> int {
        const socketConfig socket_config{ip, iface, port, false, is_listening, false};
        fd_ = createSocket(logger_, socket_config);
        sock_attrib_.sin_addr.s_addr = INADDR_ANY;
        sock_attrib_.sin_family = AF_INET;
        sock_attrib_.sin_port = htons(port);
        return fd_;
    }

    auto TCPSocket::send(const char* data, size_t len) noexcept -> void {
        memcpy(send_buffer_+next_send_valid_idx_, data, len);
        next_send_valid_idx_ += len;
    }

    auto TCPSocket::sendAndRecv() noexcept -> bool {
        char ctrl[CMSG_SPACE(sizeof(struct timeval))];
        auto cmsg = reinterpret_cast<cmsghdr*>(&ctrl);

        iovec iov{recv_buffer_ + next_recv_valid_idx_, tcpBufferSize - next_recv_valid_idx_};
        msghdr msg{&sock_attrib_, sizeof(sock_attrib_), &iov, 1, ctrl, sizeof(ctrl), 0};

        const auto read_size = recvmsg(fd_, &msg, MSG_DONTWAIT);

        if(read_size > 0) {
            next_recv_valid_idx_ += read_size;
            Nanos kernel_time = 0;
            timeval time_kernel;

            if(cmsg->cmsg_level == SOL_SOCKET &&
                cmsg->cmsg_type == SCM_TIMESTAMP &&
                cmsg->cmsg_len == CMSG_LEN(sizeof(time_kernel))
            ){
                memcpy(&time_kernel, CMSG_DATA(cmsg), sizeof(time_kernel));
                kernel_time = time_kernel.tv_sec * NANOS_TO_SECONDS + time_kernel.tv_usec * NANOS_TO_MICROS;
            }

            const auto user_time = getCurrentNanos();
            logger_.log("% : % %() read from socket: % len: % utime: % ktime: % diff: %\n", __FILE__, __LINE__, __FUNCTION__, user_time, kernel_time, (user_time - kernel_time));
            recv_callback_(this, kernel_time);
        }

        if(next_send_valid_idx_ > 0){
            const auto n = ::send(fd_, send_buffer_, next_send_valid_idx_, MSG_DONTWAIT | MSG_NOSIGNAL);
            logger_.log("% : % %() % send socket % len:%\n", __FILE__, __LINE__, __FUNCTION__, getCurrentTimeStr(&time_str_), fd_, n);
        }

        next_send_valid_idx_ = 0;

        return (read_size > 0);
    }
}