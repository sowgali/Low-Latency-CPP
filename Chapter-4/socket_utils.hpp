#pragma once

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
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
}