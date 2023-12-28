#pragma once

#include <atomic>
#include <vector>
#include "macros.hpp"

namespace Common{
    template<typename T>
    class LFQueue final {
        public:
            LFQueue() = delete;
            LFQueue(const LFQueue&) = delete;
            LFQueue(const LFQueue&&) = delete;
            LFQueue& operator=(const LFQueue&) = delete;
            LFQueue& operator=(const LFQueue&&) = delete;

            LFQueue(size_t n) : queue(n, T()){}

            auto getNextToWrite() noexcept{
                return &queue[write_idx_];
            }

            void updateWriteIdx() noexcept {
                write_idx_ = (write_idx_ + 1) % queue.size();
                qSize++;
            }

            auto getNextToRead() const noexcept -> const T* {
                return (size() ? &queue[read_idx_] : nullptr);
            }

            void updateReadIdx() noexcept {
                ASSERT(qSize > 0, "Empty queue : " + std::to_string(pthread_self()));
                read_idx_ = (read_idx_ + 1) % queue.size();
                qSize--;
            }

            auto size() const noexcept {
                return qSize.load();
            }
        
        private:
            std::vector<T> queue;
            std::atomic<size_t> write_idx_ = 0;
            std::atomic<size_t> read_idx_ = 0;
            std::atomic<size_t> qSize = 0;

    };
}