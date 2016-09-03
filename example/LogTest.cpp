#include <dlog/logger/Log.h>

using namespace dlog::logger;

int main() {
    DLOG_INIT();
    
    DLOG_LOG(WARN, "test the log level using log lib!");
    DLOG_LOG(DEBUG, "Hello World, %d",2016);
    DLOG_LOG(INFO, "test Log C/C++ lib");
    DLOG_LOG(ERROR, "Hello everyone, this is my blog: http://armsword.com/; Welcome to visit,Thank you!");
    return 0;
}
