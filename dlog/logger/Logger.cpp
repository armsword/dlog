#include <dlog/logger/Logger.h>
#include <dlog/config/ConfigParser.h>
#include <dlog/common/TimeUtility.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

DLOG_BEGIN_NAMESPACE(logger);
using namespace config;
using namespace common;

Logger::Logger() : _fd(NULL), _logLevel(LOG_LEVEL_DEBUG), _logBlockid(0)
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
        FILE *newfd = fopen(_logLocation.c_str(), "a");
        FILE *oldfd = NULL;
        {
            ScopedLock lock(_lock);
            oldfd = _fd;
            _fd = newfd;
            setBufferFormat(_asyncFlush);
        }
        close(oldfd);
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

void Logger::close(FILE *fd) {
    if(fd != NULL) {
        fflush(fd);  
        fclose(fd);
    }
}

DLOG_END_NAMESPACE(logger);
