#ifndef DLOG_LOGGER_H
#define DLOG_LOGGER_H

#include <dlog/common/Common.h>

DLOG_BEGIN_NAMESPACE(logger);

class Logger
{
public:
    Logger();
    ~Logger();
private:
    Logger(const Logger &);
    Logger& operator=(const Logger &);
public:

private:
    DLOG_LOG_DECLARE();
};

DLOG_TYPEDEF_PTR(Logger);

DLOG_END_NAMESPACE(logger);

#endif //DLOG_LOGGER_H
