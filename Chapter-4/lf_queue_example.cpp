#include "lf_queue.hpp"
#include "thread_utils.hpp"
#include <iostream>

struct data {
    int d_[3];
};

void producerFunction(Common::LFQueue<data>* lFQueue){
    for(int i = 0; i < 50; i++){
        auto loc = lFQueue->getNextToWrite();
        lFQueue->updateWriteIdx();
        loc->d_[0] = i;
        loc->d_[1] = i+1;
        loc->d_[2] = i+2;
        std::cout << "Added elem: {" << loc->d_[0] << ", " << loc->d_[1] << ", " << loc->d_[2] << "} and qSize: " << lFQueue->size() << std::endl;
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Producer exiting" << std::endl;
}

void consumerFunction(Common::LFQueue<data>* lFQueue){
    while(lFQueue->size()){
        auto loc = lFQueue->getNextToRead();
        lFQueue->updateReadIdx();
        std::cout << "Consumed elem: {" << loc->d_[0] << ", " << loc->d_[1] << ", " << loc->d_[2] << "} and qSize: " << lFQueue->size() << std::endl;
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    std::cout << "Consumer exiting" << std::endl;
}

int main(int, char**){
    Common::LFQueue<data> lfQueue(100);
    auto pt = Common::createAndStartThread(-1, "producer thread", producerFunction, &lfQueue);
    auto ct = Common::createAndStartThread(-1, "consumer thread", consumerFunction, &lfQueue);
    pt->join();
    ct->join();
    std::cout << "main exiting" << std::endl;
    return 0;
}