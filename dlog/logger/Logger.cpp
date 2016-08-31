#include <dlog/logger/Logger.h>
#include <dlog/config/ConfigParser.h>
#include <dlog/common/TimeUtility.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

DLOG_BEGIN_NAMESPACE(logger);
using namespace config;
using namespace common;

Logger::Logger() : 
    _fd(NULL), 
    _changeFd(NULL),
    _logLevel(LOG_LEVEL_DEBUG), 
    _logBlockid(0),
    _needChangeFd(false)
{ 
}

Logger::~Logger() { 
    close(_fd);
}

bool Logger::init() {
    ConfigParser parser;
    _logPath = parser.getLogPath();
    _logPrefix = parser.getLogPrefix();
    _asyncFlush = parser.getAsyncFlush();
    _maxFileSize = parser.getMaxFileSize();
    if(!createDir(_logPath)) {
        return false;
    }
    if(!openFile(_logPath, _logPrefix)) {
        return false;
    }
    setBufferFormat(_asyncFlush);
    if(!createLoopThread()) {
        std::stringstream pid;
        pid << getpid();
        std::string defaultOutput = std::string("stderr") + "." + pid.str();
        freopen(defaultOutput.c_str(), "w", stderr);
        fprintf(stderr, "create loop thread failed! \n");
        fclose(stderr);
        return false;
    }
    return true;
}

void Logger::log(LogLevel level, const char * file, uint32_t line, 
                 const char * func, char* format, ...) 
{
    char *buffer = _buffer;
    uint32_t logLen = 0;
    uint32_t headLen  = prepareLogHead(buffer, file, line, func);
    va_list ap;
    va_start(ap, format);
    logLen = vsnprintf(buffer, DEFAULT_BUFFER_SIZE - headLen, format, ap);
    va_end(ap);
    if(likely(_fd != NULL)) {
        dump(headLen + logLen);
    }
}

void Logger::dump(uint32_t len) {
    if(likely(access(_logLocation.c_str(), W_OK) == 0)) {
        if(likely(_needChangeFd == false)) {
            if(fwrite(_buffer, len, 1, _fd) == 1) {
                *_buffer = '\0';  //重置buffer
            } 
            else {
                std::stringstream pid;
                pid << getpid();
                std::string defaultOutput = std::string("stderr") + "." + pid.str();
                freopen(defaultOutput.c_str(), "w", stderr);
                fprintf(stderr, "create loop thread failed! \n");
                fclose(stderr);
            }
        } else {
            {
                ScopedLock lock(_lock);
                FILE *tempFd = NULL;
                tempFd = _fd;
                _fd = _changeFd;
                _changeFd = tempFd;
                _needChangeFd = false;
                setBufferFormat(_asyncFlush);
            }
        }
    }

}

LogLevel Logger::getLogLevel() const {
    return _logLevel;
}

const char* Logger::logLevelToString(LogLevel level) const {
    switch(level) {
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    case LOG_LEVEL_INFO:
        return "INFO";
    case LOG_LEVEL_WARN:
        return "WARN";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    default:
        return "DEBUG";     // if the level is wrong, use DEBUG instead
    }
}

bool Logger::createDir(const std::string& logPath) const {
    if(access(logPath.c_str(), F_OK) == -1) {
        if(mkdir(logPath.c_str(), 0755) < 0) {
            config::ConfigParser::defaultOutput(logPath);
            return false;
        }
    }
    return true;
}

bool Logger::openFile(const std::string &logPath, const std::string &logPrefix)
{
    std::stringstream logBlockid;
    logBlockid << _logBlockid;
    _logLocation = logPath + logPrefix + ".log." + 
                   common::TimeUtility::currentTimeString() + logBlockid.str();
    _fd = fopen(_logLocation.c_str(), "a");
    if(_fd == NULL) {
        config::ConfigParser::defaultOutput(_logLocation);
        return false;
    }
    return true;
}

void Logger::setBufferFormat(bool asyncFlush) {
    // asyncFlush为true时设置行缓冲，否则无缓冲
    if(asyncFlush) {
        setvbuf(_fd, NULL, _IOLBF, 0);
    } else {
        setvbuf(_fd, NULL, _IONBF, 0);
    }
}

bool Logger::createLoopThread() {
    _checkThreadPtr = LoopThread::createLoopThread(
            std::bind(&Logger::checkFile, this),
            DEFAULT_CHECK_INTERVAL);
    return _checkThreadPtr != NULL;
}

void Logger::checkFile() {
    uint32_t fileSize = getLogBlockSize();
    if(fileSize >= _maxFileSize * 1024 * 1024) {   
        _logBlockid += 1;
        std::stringstream logBlockid;
        logBlockid << _logBlockid;
        _logLocation = _logPath + _logPrefix + ".log." + 
                       common::TimeUtility::currentTimeString() + logBlockid.str();
        _changeFd = fopen(_logLocation.c_str(), "a");
        _needChangeFd = true;
        
        if(!_needChangeFd) {
            if(_changeFd != NULL) {
                close(_changeFd);
            }
        }
    }
}

uint32_t Logger::getLogBlockSize() const {
    uint32_t fileSize = 0;
    struct stat logStat;
    if(stat(_logLocation.c_str(), &logStat)) {
        return fileSize;
    }
    return logStat.st_size;
}

uint32_t Logger::prepareLogHead(char *buffer, const char* file,
                                uint32_t line, const char *func) const {
    buffer[0] = '[';
    uint32_t timeOffset = common::TimeUtility::getCurTime(buffer + 1, DEFAULT_BUFFER_SIZE);
    buffer[timeOffset + 1] = ']';
    uint32_t len = timeOffset + 2;
    return snprintf(buffer + len, DEFAULT_BUFFER_SIZE - len, " [%s] [%d] [%s:%d] [%s] ",
                    logLevelToString(_logLevel), (int)pthread_self(), file, line, func);

}

void Logger::close(FILE *fd) {
    if(fd != NULL) {
        fflush(fd);  
        fclose(fd);
    }
}

DLOG_END_NAMESPACE(logger);
