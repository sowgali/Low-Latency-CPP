#pragma once


#include <iostream>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <sched.h>
#include <sys/syscall.h>

namespace Common{
    inline auto setThreadCore(int coreId) noexcept {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        CPU_SET(coreId, &cpuset);

        return(pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0);
    }

    template<typename T, typename... A>
    inline auto createAndStartThread(int coreId, std::string name, T&& func, A&&... args) noexcept {
        auto t = new std::thread([&]() {
            if(coreId >= 0 && !setThreadCore(coreId)){
                std::cerr << "Failed to set thread affinity to " << coreId << " for " << name << " " << pthread_self() << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cerr << "Successfully set thread affinity to " << coreId << " for " << name << " " << pthread_self() << std::endl;
            std::forward<T>(func)((std::forward<A>(args))...);
        });

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);

        return t;
    }


}