#pragma once

#include <string>
#include <fstream>
#include <cstdio>

#include "macros.hpp"
#include "thread_utils.hpp"
#include "lf_queue.hpp"
#include "time_utils.hpp"

namespace Common {
    constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;

    enum class LogType : int8_t {
        CHAR = 0,
        INTEGER = 1,
        LONG_INTEGER = 2,
        LONG_LONG_INTEGER = 3,
        UNSIGNED_INTEGER = 4,
        UNSIGNED_LONG_INTEGER = 5,
        UNSIGNED_LONG_LONG_INTEGER = 6,
        FLOAT = 7,
        DOUBLE = 8
    };

    struct LogElement {
        LogType logType_ = LogType::CHAR;
        union {
            char c;
            int i;
            long l;
            long long ll;
            unsigned u;
            unsigned long ul;
            unsigned long long ull;
            float f;
            double d;
        } u_;
    };

    class Logger final {
        public:
            auto flushQueue() noexcept {
                while(running_){
                    for(auto next = queue_.getNextToRead(); queue_.size() && next; next = queue_.getNextToRead()){
                        switch (next->logType_) {
                            case LogType::CHAR:
                                fout << next->u_.c;
                                break;
                            case LogType::INTEGER:
                                fout << next->u_.i;
                                break;
                            case LogType::LONG_INTEGER:
                                fout << next->u_.l;
                                break;
                            case LogType::LONG_LONG_INTEGER:
                                fout << next->u_.ll;
                                break;
                            case LogType::UNSIGNED_INTEGER:
                                fout << next->u_.u;
                                break;
                            case LogType::UNSIGNED_LONG_INTEGER:
                                fout << next->u_.ul;
                                break;
                            case LogType::UNSIGNED_LONG_LONG_INTEGER:
                                fout << next->u_.ull;
                                break;
                            case LogType::FLOAT:
                                fout << next->u_.f;
                                break;
                            case LogType::DOUBLE:
                                fout << next->u_.d;
                                break;
                        }
                        queue_.updateReadIdx();
                    }
                    fout.flush();

                    using namespace std::literals::chrono_literals;
                    std::this_thread::sleep_for(10ms);
                }
            }

            Logger(const std::string& fname) : filename(fname), queue_(LOG_QUEUE_SIZE) {
                fout.open(fname);
                ASSERT(fout.is_open(), "The log file is not open");
                logger_thread_ = createAndStartThread(-1, "logger thread", [this](){
                    flushQueue();
                });
                ASSERT(logger_thread_ != nullptr, "Failed to start logger thread");
            }

            ~Logger(){
                std::string time_str;
                std::cerr << Common::getCurrentTimeStr(&time_str) << " Flushing and closing logger for " << filename << std::endl;
                while(queue_.size()){
                    using namespace std::literals::chrono_literals;
                    std::this_thread::sleep_for(1s);
                }
                running_ = false;
                logger_thread_->join();
                fout.close();

                std::cerr << Common::getCurrentTimeStr(&time_str) << " Logger for " << filename << " exiting." << std::endl;
            }


            auto pushValue(const LogElement &log_element) noexcept {
                *(queue_.getNextToWrite()) = log_element;
                queue_.updateWriteIdx();
            }

            auto pushValue(const char value) noexcept {
                pushValue(LogElement{LogType::CHAR, {.c = value}});
            }

            auto pushValue(const int value) noexcept {
                pushValue(LogElement{LogType::INTEGER, {.i = value}});
            }

            auto pushValue(const long value) noexcept {
                pushValue(LogElement{LogType::LONG_INTEGER, {.l = value}});
            }

            auto pushValue(const long long value) noexcept {
                pushValue(LogElement{LogType::LONG_LONG_INTEGER, {.ll = value}});
            }

            auto pushValue(const unsigned value) noexcept {
                pushValue(LogElement{LogType::UNSIGNED_INTEGER, {.u = value}});
            }

            auto pushValue(const unsigned long value) noexcept {
                pushValue(LogElement{LogType::UNSIGNED_LONG_INTEGER, {.ul = value}});
            }

            auto pushValue(const unsigned long long value) noexcept {
                pushValue(LogElement{LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull = value}});
            }

            auto pushValue(const float value) noexcept {
                pushValue(LogElement{LogType::FLOAT, {.f = value}});
            }

            auto pushValue(const double value) noexcept {
                pushValue(LogElement{LogType::DOUBLE, {.d = value}});
            }

            auto pushValue(const char *value) noexcept {
                while (*value) {
                    pushValue(*value);
                    ++value;
                }
            }
            /**
             * TODO: Improve this using memcopy
            */
            auto pushValue(const std::string &value) noexcept {
                pushValue(value.c_str());
            }

            template<typename T, typename... Args>
            auto log(const char* str, const T& val, Args... args) noexcept {
                while(*str){
                    if(*str == '%'){
                        if(*(str+1) == '%')[[unlikely]]{
                            str++;
                        } else {
                            pushValue(val);
                            log(str+1, args...);
                            return;
                        }
                    }
                    pushValue(*str++);
                }
                FATAL("Extra arguments provided to log() function");
            }
            
            auto log(const char* str) noexcept {
                while(*str){
                    if(*str == '%'){
                        if(*(str+1) == '%')[[unlikely]]{
                            str++;
                        } else {
                            FATAL("Missing arguments after %");
                        }
                    }
                    pushValue(*str++);
                }
            }


            Logger() = delete;
            Logger(const Logger&) = delete;
            Logger(const Logger&&) = delete;
            Logger& operator=(const Logger&) = delete;
            Logger& operator=(const Logger&&) = delete;

        private:
            const std::string filename;
            std::ofstream fout;
            LFQueue<LogElement> queue_;
            std::atomic<bool> running_ = {true};
            std::thread* logger_thread_ = nullptr;
    };
}