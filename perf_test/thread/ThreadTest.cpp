#include <dlog/logger/Log.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <pthread.h>

using namespace dlog::logger;

int64_t currentTime() {
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec) / 1000000;   // 返回秒
}

void *print(void* buffer) {
    for(int i = 0; i < 1500000; i++) {
        DLOG_LOG(DEBUG, "%s", (char*)buffer);
    }
    return ((void*)0);
}

int main() {
    DLOG_INIT();
    pthread_t tid[2];
    char buffer[256];
    int i;
    for(i = 0; i < 255; i++) {
        buffer[i] = 'A' + rand() % 26;
    }
    buffer[++i] = '\0';

    int64_t begin = currentTime();
    for(i = 0; i < 2; i++) {
        if(pthread_create(&tid[i],NULL,print,buffer) != 0) {
            std::cout << "create thread error!" << std::endl;
            exit(1);
        }
    }

    for(i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    int64_t end = currentTime();
    int64_t total = end - begin;
    std::cout << "total time is: " << total << std::endl;
    return 0;
}
