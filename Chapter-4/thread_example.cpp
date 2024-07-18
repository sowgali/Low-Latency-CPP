#include "thread_utils.hpp"

inline auto AddFunction(const int a, const int b, const bool sleep) noexcept {
    std::cout << "AddFunction(" << a << ", " << b << ", " << std::boolalpha << sleep << ")" << std::endl;
    std::cout << "AddFunction result: " << a+b << std::endl;
    if(sleep){
        std::cout << "AddFunction sleeping ..." << std::endl;
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(5s);
    }
    std::cout << "AddFunction done." << std::endl;
}

int main(){
    using namespace Common;
    auto t1 = createAndStartThread(-1, "Thread-1 Core -1", AddFunction, 1, 2, true);
    auto t2 = createAndStartThread(1, "Thread-2 Core 1", AddFunction, 1, 2, false);
    std::cout << "Waiting for threads to finish" << std::endl;
    t1->join();
    t2->join();
    std::cout << "Threads finished" << std::endl;
    return 0;
}