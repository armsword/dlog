#include "div.h"
#include <dlog/logger/Log.h>

Test::Test() {
}
Test::~Test() {

}
int Test::div(int x, int y) {
    DLOG_LOG(ERROR, "test div result\n");
    std::cout << "test div" << std::endl;
    return x / y;
}

// int main() {
//     Test div;
//     div.div(6,3);
//     return 0;
// }
