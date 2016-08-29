#ifndef DLOG_LOGGER_H
#define DLOG_LOGGER_H

#include <dlog/common/Common.h>
#include <dlog/common/Lock.h>
#include <dlog/common/LoopThread.h>

DLOG_BEGIN_NAMESPACE(logger);

class Logger
{ 
public:
    Logger();
    ~Logger();
public:
    const static uint32_t DEFAULT_CHECK_INTERVAL = 500 * 1000; // 500 ms
private:
    Logger(const Logger &);
    Logger& operator=(const Logger &);
public:
    bool init();

private:
    const char* logLevelToString(LogLevel level) const;
    bool createDir(const std::string &logPath) const;
    bool openFile(const std::string &logPath, const std::string &logPrefix);
    void setBufferFormat(bool asyncFlush);
    bool createLoopThread();
    void checkFile();
    uint32_t getLogBlockSize() const;
    void close(FILE *fd);

private:
    FILE *_fd;
    LogLevel _logLevel;
    uint32_t _maxFileSize;
    std::string _logLocation;
    uint32_t _logBlockid;
    bool _asyncFlush;
    std::string _logPath;
    std::string _logPrefix;
    common::ThreadMutex _lock;
    common::LoopThreadPtr _checkThreadPtr;
    static __thread char _buffer[DEFAULT_BUFFER_SIZE];
};

DLOG_TYPEDEF_PTR(Logger);

DLOG_END_NAMESPACE(logger);

#endif //DLOG_LOGGER_H
