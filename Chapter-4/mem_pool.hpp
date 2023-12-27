#pragma once

#include <vector>
#include "macros.hpp"

namespace Common {
    template<typename T>
    class MemPool final {
        public:
            MemPool() = delete;
            MemPool(const MemPool&) = delete;
            MemPool(const MemPool&&) = delete;
            MemPool& operator=(const MemPool&) = delete;
            MemPool& operator=(const MemPool&&) = delete;

            MemPool(size_t n) : pool_(std::vector<ObjBlock>(n, {T(), true})){
                ASSERT(reinterpret_cast<const ObjBlock*>(&(pool_[0].object_)) == &(pool_[0]), "Expected type T as first member of object block");
            }

            template<typename... Args>
            T* allocate(Args... args) noexcept {
                auto objBlock = &(pool_[next_free_idx_]);
                ASSERT(objBlock->is_free_, "Expected free block at: " + std::to_string(next_free_idx_));
                T* ret = new (objBlock) T(args...);
                objBlock->is_free_ = false;
                updateNextFreeIdx();
                return ret;
            }

            auto deallocate(const T* objBlock) noexcept {
                const auto idx = reinterpret_cast<const ObjBlock*>(objBlock) - (&pool_[0]);
                ASSERT(idx >= 0 && static_cast<size_t>(idx) < pool_.size(), "This object at: " + std::to_string(idx) + " does not belong to this pool");
                ASSERT(!pool_[idx].is_free_, "Non-empty block is expected at index: " + std::to_string(idx));
                pool_[idx].is_free_ = true;
            }
        
        private:
            auto updateNextFreeIdx() noexcept {
                const auto init_free_idx_ = next_free_idx_;
                while(!pool_[next_free_idx_].is_free_){
                    next_free_idx_++;
                    if(next_free_idx_ == pool_.size())[[unlikely]]{
                        next_free_idx_ = 0;
                    }
                    if(next_free_idx_ == init_free_idx_)[[unlikely]]{
                        ASSERT(next_free_idx_ != init_free_idx_, "Memory pool out of space");
                    }
                }
            }
            struct ObjBlock {
                T object_;
                bool is_free_ = true;
            };
            std::vector<ObjBlock> pool_;
            size_t next_free_idx_ = 0;
    };
}
