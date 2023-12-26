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
}