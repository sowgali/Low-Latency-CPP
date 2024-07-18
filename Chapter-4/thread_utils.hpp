#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>

namespace Common{
    inline auto setAffinity(int core_id) noexcept {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
    }

    template<typename T, typename... A>
    inline auto createAndStartThread(int core_id, const std::string& name, T&& func, A&&... args){
        auto t = new std::thread([&](){
            if(core_id >= 0 && !setAffinity(core_id)){
                std::cerr << "Failed to set thread affinity for " << name << " " << pthread_self() << " to core " << core_id << std::endl;
                exit(EXIT_FAILURE);
            }
            std::cerr << "Set thread affinity for " << name << " " << pthread_self() << " to core " << core_id << std::endl;
            std::forward<T>(func)((std::forward<A>(args))...);
            {
                using namespace std::literals::chrono_literals;
                std::this_thread::sleep_for(1s);
            }
        });
        return t;
    }
}