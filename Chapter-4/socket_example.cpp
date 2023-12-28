#include "socket_utils.hpp"
#include <iostream>

int main(int, char**){
    std::cout << Common::getIfaceIP("lo") << std::endl;
    std::cout << Common::getIfaceIP("enp0s1") << std::endl;
    return 0;
}