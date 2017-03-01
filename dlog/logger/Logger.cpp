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
const static std::string DEFAULT_JSON_STRING = "dlog.json";
__thread char Logger::_buffer[DEFAULT_BUFFER_SIZE];

Logger::Logger() : 
    _fd(NULL), 
    _changeFd(NULL),
    _logLevel(LOG_LEVEL_DEBUG), 
    _logLocation(""),
    _logBlockid(0),
    _needChangeFd(false)
{ 
}

Logger::~Logger() { 
    close(_fd);
}

bool Logger::init() {
    if (likely(access(_logLocation.c_str(), W_OK) == 0)) { 
        print("the log has init once, you do not need init twice! \n");
        return true;   //避免多次初始化
    }
    ConfigParser parser;
    parser.parse(DEFAULT_JSON_STRING);
    _logPath = parser.getLogPath();
    _logPrefix = parser.getLogPrefix();
    _logLevel = parser.getLogLevel();
    _asyncFlush = parser.getAsyncFlush();
    _maxFileSize = parser.getMaxFileSize();
    if (!createDir(_logPath)) {
        return false;
    }
    if (!openFile()) {
        return false;
    }
    setBufferFormat(_asyncFlush);
    if (!createLoopThread()) {
        print("create loop thread failed! \n");
        return false;
    }
    return true;
}

void Logger::log(LogLevel level, const char * file, uint32_t line, 
                 const char * func, const char* format, ...) 
{
    char *buffer = _buffer;
    uint32_t logLen = 0;
    uint32_t totalLen = 0;
    uint32_t headLen  = prepareLogHead(buffer, file, line, func, level);
    
    va_list ap;
    va_start(ap, format);
    logLen = vsnprintf(buffer + headLen, DEFAULT_BUFFER_SIZE - headLen, format, ap);
    va_end(ap);
    totalLen = headLen + logLen;
    buffer[totalLen] = '\n';
    buffer[totalLen + 1] = '\0';
    if (likely(_fd != NULL)) {
        dump(totalLen + 1);
    }
}

void Logger::dump(uint32_t len) {
    if (likely(_needChangeFd == false)) {
        if (fwrite(_buffer, len, 1, _fd) == 1) {
            *_buffer = '\0';  //重置buffer
        } 
        else {
            print("dump buffer to disk failed! \n");
        }
    } else {
        {
            ScopedLock lock(_lock);
            if (_needChangeFd) {   // 需要多一次判断，否则2个线程都走到临界区时，交换fd会出现bug
                FILE *tempFd = NULL;
                tempFd = _fd;
                _fd = _changeFd;
                _changeFd = tempFd;
                _needChangeFd = false;
                fwrite(_buffer, len, 1, _changeFd);
                fflush(_changeFd);
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
    if (access(logPath.c_str(), F_OK) == -1) {
        if(mkdir(logPath.c_str(), 0755) < 0) {
            config::ConfigParser::defaultOutput(logPath);
            return false;
        }
    }
    return true;
}

bool Logger::openFile()
{
    std::stringstream logBlockid;
    _currentDay = common::TimeUtility::currentDay();
    logBlockid << _logBlockid;
    _logLocation = _logPath + '/' + _logPrefix + ".log." + 
                   _currentDay + '.' + logBlockid.str();
    _fd = fopen(_logLocation.c_str(), "a");
    if (_fd == NULL) {
        config::ConfigParser::defaultOutput(_logLocation);
        return false;
    }
    return true;
}

void Logger::setBufferFormat(bool asyncFlush) {
    // asyncFlush为true时设置行缓冲，否则无缓冲
    if (asyncFlush) {
        setvbuf(_fd, NULL, _IOLBF, 0);
    } else {
        setvbuf(_fd, NULL, _IONBF, 0);
    }
}

bool Logger::createLoopThread() {
    _checkThreadPtr = LoopThread::createLoopThread(
            std::tr1::bind(&Logger::checkFile, this),
            DEFAULT_CHECK_INTERVAL);
    return _checkThreadPtr != NULL;
}

void Logger::checkFile() {
    uint64_t fileSize = getLogBlockSize();
    if (fileSize >= _maxFileSize * 1024 * 1024 || 
       _currentDay != common::TimeUtility::currentDay()) 
    {   
        _logBlockid += 1;
        std::stringstream logBlockid;
        logBlockid << _logBlockid;
        _logLocation = _logPath + '/' + _logPrefix + ".log." + 
                       common::TimeUtility::currentDay() + '.' + logBlockid.str();
        _changeFd = fopen(_logLocation.c_str(), "a");
        if (_changeFd == NULL) {
            print("open new file error!\n"); 
        }
        _needChangeFd = true;
        
        if (!_needChangeFd) {
            if(_changeFd != NULL) {
                close(_changeFd);
            }
        }
    }
}

uint32_t Logger::getLogBlockSize() const {
    uint64_t fileSize = 0;
    struct stat logStat;
    if (stat(_logLocation.c_str(), &logStat)) {
        return fileSize;
    }
    return logStat.st_size;
}

uint32_t Logger::prepareLogHead(char *buffer, const char* file, uint32_t line, 
                                const char *func, LogLevel level) const {
    buffer[0] = '[';
    uint32_t timeOffset = common::TimeUtility::getCurTime(buffer + 1, DEFAULT_BUFFER_SIZE);
    buffer[timeOffset + 1] = ']';
    uint32_t len = timeOffset + 2;
    return len + snprintf(buffer + len, DEFAULT_BUFFER_SIZE - len, " [%s] [%u] [%s:%d] [%s] ",
                          logLevelToString(level), (int)pthread_self(), file, line, func);

}

void Logger::print(const std::string &output) {
    std::stringstream pid;
    pid << getpid();
    std::string defaultOutput = std::string("stderr") + "." + pid.str();
    freopen(defaultOutput.c_str(), "w", stderr);
    fprintf(stderr, output.c_str());
    fclose(stderr);
}

void Logger::close(FILE *fd) {
    if (fd != NULL) {
        fflush(fd);  
        fclose(fd);
    }
}

DLOG_END_NAMESPACE(logger);
