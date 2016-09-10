#include <dlog/logger/Log.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <iostream>
#include <string.h>

using namespace dlog::logger;

int64_t currentTime() {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec) / 1000000;   // 返回秒
}

int main() {
    DLOG_INIT();
    
    char buffer[256];
    int i;
    for(i = 0; i < 255; i++) {
        buffer[i] = 'A' + rand() % 26;
    }
    buffer[++i] = '\0';

    int64_t begin = currentTime();
    for(int i = 0; i < 10000000; i++) {
        DLOG_LOG(DEBUG, "%s", buffer);
    }
    int64_t end = currentTime();
    int64_t total = end - begin;
    std::cout << "total time is: " << total << std::endl;
    return 0;
}
