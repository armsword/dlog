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
    void log(LogLevel level, const char * file, uint32_t line, 
             const char * func, const char* format, ...);
    LogLevel getLogLevel() const;

private:
    const char* logLevelToString(LogLevel level) const;
    LogLevel stringToLogLevel(const std::string &logLevel) const;
    bool createDir(const std::string &logPath) const;
    bool openFile();
    void setBufferFormat(bool asyncFlush);
    bool createLoopThread();
    void checkFile();
    uint32_t getLogBlockSize() const;
    uint32_t prepareLogHead(char *buffer, const char* file, uint32_t line, 
                            const char *func, LogLevel level) const;
    void close(FILE *fd);    
    void dump(uint32_t len);
    
private:
    FILE *_fd;
    FILE *_changeFd;
    LogLevel _logLevel;
    uint32_t _maxFileSize;
    std::string _logLocation;
    uint32_t _logBlockid;
    bool _asyncFlush;
    std::string _logPath;
    std::string _logPrefix;
    bool _needChangeFd;
    common::ThreadMutex _lock;
    common::LoopThreadPtr _checkThreadPtr;
    static __thread char _buffer[DEFAULT_BUFFER_SIZE];
};

DLOG_TYPEDEF_PTR(Logger);

DLOG_END_NAMESPACE(logger);

#endif //DLOG_LOGGER_H
