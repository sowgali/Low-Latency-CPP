#include "mem_pool.hpp"

struct myData {
    int d_[3];
};

int main(int, char**){
    using namespace Common;
    MemPool<float> floatPool(100);
    MemPool<myData> myDataPool(100);
    for(int i = 0; i < 100; i++){
        auto floatRet = floatPool.allocate(i);
        auto structRet = myDataPool.allocate(myData{i,i+1,i+2});
        std::cout << "Float element: " << *floatRet << " at " << floatRet << std::endl;
        std::cout << "Struct element: {" << structRet->d_[0] << ", " << structRet->d_[1] << ", "  <<  structRet->d_[2] << "} at " << structRet << std::endl;

        if(i % 5  == 0){
            std::cout << "Deallocating float element: " << *floatRet << " from " << floatRet << std::endl;
            std::cout << "Deallocating struct element: {" << structRet->d_[0] << ", " << structRet->d_[1] << ", "  <<  structRet->d_[2] << "} from " << structRet << std::endl;
            floatPool.deallocate(floatRet);
            myDataPool.deallocate(structRet);
        }
    }
    return 0;
}