#ifndef DLOG_LOG_H
#define DLOG_LOG_H

#include <dlog/common/Common.h>
#include <dlog/logger/Logger.h>

//static dlog::logger::Logger logger;
struct Log {
    Log() {
        logger.init();
    }
    dlog::logger::Logger logger;
};

inline dlog::logger::Logger& getLogInstance() {
    static Log logInstance;
    return logInstance.logger;
}

#define DLOG_INIT()                             \
    do {                                        \
        getLogInstance();                       \
    } while(0)

#define DLOG_LOG(level, format, args...)                                \
    do {                                                                \
        if (getLogInstance().getLogLevel() <= dlog::LOG_LEVEL_##level)   \
        {                                                               \
            getLogInstance().log(dlog::LOG_LEVEL_##level, __FILE__, __LINE__, \
                       __FUNCTION__,format, ##args);                    \
        }                                                               \
    } while(0)

#endif //DLOG_LOG_H
