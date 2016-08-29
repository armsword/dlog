#ifndef DLOG_COMMONDEFINE_H
#define DLOG_COMMONDEFINE_H
#include <string>
#include <map>
#include <set>
#include <vector>
#include <limits>

DLOG_NS_BEGIN;

enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_UNKNOWN = 1,
    ERROR_FILE_OPEN = 2,
    ERROR_JSON_PARSE = 3
};

enum LogLevel {
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARN = 3,
    LOG_LEVEL_ERROR = 4
};

typedef std::map<std::string,std::string> StringMap;
typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;

#define INVALID_NODEPATH_COUNT (std::numeric_limits<uint32_t>::max())

const static uint32_t DEFAULT_TIMEOUT = 10000;
const static uint32_t DEFAULT_BUFFER_SIZE = 4 * 1024 * 1024; // 4M



DLOG_NS_END;

#endif //DLOG_COMMONDEFINE_H
