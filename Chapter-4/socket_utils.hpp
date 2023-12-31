#pragma once

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <sstream>

#include "logging.hpp"
#include "time_utils.hpp"
#include "macros.hpp"

namespace Common {

    struct socketConfig {
        std::string ip_;
        std::string iface_;
        int port_ = -1;
        bool is_udp_ = false;
        bool is_listening_ = false;
        bool needs_so_timestamp_ = false;

        auto toString() const {
            std::stringstream ss;
            ss << "SocketCfg[ip:" << ip_
            << " iface:" << iface_
            << " port:" << port_
            << " is_udp:" << is_udp_
            << " is_listening:" << is_listening_
            << " needs_SO_timestamp:" << needs_so_timestamp_
            << "]";

            return ss.str();
        }
    };

    constexpr int MaxTCPServerBacklog = 1024;

    inline auto getIfaceIP(const std::string& iface) -> std::string {
        char buf[NI_MAXHOST] = {'\0'};
        ifaddrs* ifaddr = nullptr;
        if(getifaddrs(&ifaddr) != -1){
            for(ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next){
                if(ifa && ifa->ifa_addr->sa_family == AF_INET && iface == ifa->ifa_name){
                    getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
                    break;
                }
            }
            freeifaddrs(ifaddr);
        }
        return buf;
    }

    inline auto setNonBlocking(int fd) -> bool {
        auto flags = fcntl(fd, F_GETFL, 0);
        if(flags == -1)
            return false;
        if(flags & O_NONBLOCK)
            return true;
        return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);
    }
    // Disables naggling
    inline auto setNoDelay(int fd) -> bool {
        int one = 1;
        return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void*>(&one), sizeof(one)) != -1);
    }

    inline auto setSOTimestamps(int fd) -> bool {
        int one = 1;
        return (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void*>(&one), sizeof(one)) != -1);
    }

    inline auto wouldBlock() -> bool {
        return (errno == EWOULDBLOCK || errno == EINPROGRESS);
    }

    inline auto setTTL(int fd, int ttl) -> bool {
        return (setsockopt(fd, IPPROTO_IP, IP_TTL, reinterpret_cast<void*>(&ttl), sizeof(ttl)) != -1);
    }

    inline auto setMcastTTL(int fd, int mcast_ttl) -> bool {
        return (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, reinterpret_cast<void*>(&mcast_ttl), sizeof(mcast_ttl)) != -1);
    }

    inline auto join(int fd, const std::string& ip) -> bool {
        ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
        return (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<void*>(&mreq), sizeof(mreq)));
    }

    [[nodiscard]] inline auto createSocket(Logger& logger, const socketConfig& socketConfig) -> int {
        std::string time_str;
        logger.log("% : % %() % cfg: %\n", __FILE__, __LINE__, __FUNCTION__, Common::getCurrentTimeStr(&time_str), socketConfig.toString());

        const int input_flags = (socketConfig.is_listening_ ? AI_PASSIVE : 0) | AI_NUMERICHOST | AI_NUMERICHOST;
        const addrinfo hints{input_flags, AF_INET, socketConfig.is_udp_ ? SOCK_DGRAM : SOCK_STREAM, socketConfig.is_udp_ ? IPPROTO_UDP : IPPROTO_TCP, 0, 0, nullptr, nullptr};
        addrinfo* result = nullptr;
        const auto rc = getaddrinfo(socketConfig.ip_.c_str(), std::to_string(socketConfig.port_).c_str(), &hints, &result);
        ASSERT(!rc, "getaddrinfo() failed. Error: " + std::string(gai_strerror(rc)) + " errno: " + std::string(strerror(errno)));

        int sockfd = -1;
        int one = 1;

        for(addrinfo* rp = result; rp; rp = result->ai_next){
            ASSERT((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) != -1, "socket() failed. errno: " + std::string(strerror(errno)));
            ASSERT(setNonBlocking(sockfd), "setNonBlocing() failed. errorno: " + std::string(strerror(errno)));
            if(!socketConfig.is_udp_){
                ASSERT(setNoDelay(sockfd), "setNoDelay() failed. errorno: " + std::string(strerror(errno)));
            }

            if(!socketConfig.is_listening_){
                ASSERT(connect(sockfd, rp->ai_addr, rp->ai_addrlen) != 1, "connect() failed. errorno: " + std::string(strerror(errno)));
            }

            if(socketConfig.is_listening_){
                ASSERT(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&one), sizeof(one)) == 0, "setsockopt() SO_REUSEADDR failed. errno: " + std::string(strerror(errno)));

                const sockaddr_in addr{AF_INET, htons(socketConfig.port_), {htonl(INADDR_ANY)}, {}};

                ASSERT(bind(sockfd, socketConfig.is_udp_ ? reinterpret_cast<const struct sockaddr*>(&addr) : rp->ai_addr, sizeof(addr)) == 0, "bind() failed. errno: " + std::string(strerror(errno)));
            }

            if(!socketConfig.is_udp_ && socketConfig.is_listening_){
                ASSERT(listen(sockfd, MaxTCPServerBacklog) == 0, "listen() failed. errno: " + std::string(strerror(errno)));
            }

            if(socketConfig.needs_so_timestamp_){
                ASSERT(setSOTimestamps(sockfd), "setSOTimestamps() failed. errno: " + std::string(strerror(errno)));
            }
        }

        return sockfd;
    }

}