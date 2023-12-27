#include "thread_utils.hpp"

auto dummyFunction(int a, int b, bool sleep){
    std::cout << "dummyFunction(" << a << ", " << b << ")" << std::endl;
    std::cout << "dummyFunction output: " << a+b << std::endl;

    if(sleep){
        std::cout << "dummyFunction sleep ..." << std::endl;

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(5s);
    }

    std::cout << "dummyFunction done." << std::endl;
}

int main(int, char**){
    using namespace Common;
    auto t1 = createAndStartThread(-1, "d1", dummyFunction, 2, 3, false);
    auto t2 = createAndStartThread(5, "d2", dummyFunction, 4, 6, true);

    std::cout << "main waiting for threads to finish" << std::endl;
    t1->join();
    t2->join();
    std::cout << "main done." << std::endl;
    return 0;
}