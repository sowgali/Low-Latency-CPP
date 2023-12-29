#pragma once

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <string>

namespace Common {
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
}