#include "logging.hpp"

int main(int, char**){
    using namespace Common;
    Logger logger("logger.log");
    
    long long ll_ = 12345;
    unsigned int ui_ = 34;
    std::string s_ = "Sowrya";
    const char* ccp_ = "Gali";
    char c_ = 'p';
    float f_ = 1.2f;
    int i_ = 4;
    long int li_ = 1234567890;
    double d_ = 1.2345;

    logger.log("Logging long long: % , unsigned int: %, string: %\n", ll_, ui_, s_);
    logger.log("Logging const char*: % , char: %, float: %\n", ccp_, c_, f_);
    logger.log("Logging int: % , long int: %, double: %\n", i_, li_, d_);

    return 0;
}