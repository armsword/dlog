#ifndef DLOG_LOG_H
#define DLOG_LOG_H

#include <dlog/common/Common.h>
#include <dlog/logger/Logger.h>

static dlog::logger::Logger logger;

#define DLOG_INIT()                             \
    do {                                        \
        logger.init();                          \
    } while(0)

#define DLOG_LOG(level, format, args...)                                \
    do {                                                                \
        if(logger.getLogLevel() <= dlog::LOG_LEVEL_##level)            \
        {                                                               \
            logger.log(dlog::LOG_LEVEL_##level, __FILE__, __LINE__,     \
                       __FUNCTION__,format, ##args);                    \
        }                                                               \
    } while(0)

#endif //DLOG_LOG_H
