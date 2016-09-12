#include <dlog/config/ConfigParser.h>
#include <dlog/common/FileUtil.h>
#include <unistd.h>
#include <algorithm>

DLOG_BEGIN_NAMESPACE(config);

ConfigParser::ConfigParser() { 
}

ConfigParser::~ConfigParser() { 
}

ErrorCode ConfigParser::parse(const std::string &path) {
    Json::Reader reader;
    Json::Value root;
    std::string content;
    bool succ = common::FileUtil::readFileContent(path,content);
    if(!succ) {
        defaultOutput(path);
        return ERROR_FILE_OPEN;
    }
    if(!reader.parse(content,root)) {
        defaultOutput(path);
        return ERROR_JSON_PARSE;
    }
    _logPath = root.get("log_path", ".").asString();
    _logPrefix = root.get("log_prefix", "dlog").asString();
    _logLevel = root.get("log_level", "DEBUG").asString();
    _maxFileSize = root.get("max_file_size", 100).asUInt();
    _asyncFlush = root.get("async_flush", true).asBool();
    return ERROR_NONE;
}

const std::string& ConfigParser::getLogPath() const {
    return _logPath;
}

const std::string& ConfigParser::getLogPrefix() const {
    return _logPrefix;
}

uint32_t ConfigParser::getMaxFileSize() const {
    return _maxFileSize;
}

bool ConfigParser::getAsyncFlush() const {
    return _asyncFlush;
}

LogLevel ConfigParser::getLogLevel() {
    transform(_logLevel.begin(), _logLevel.end(), _logLevel.begin(), toupper);    
    if(0 == _logLevel.compare("DEBUG")) {
        return LOG_LEVEL_DEBUG;
    } else if(0 == _logLevel.compare("INFO")) {
        return LOG_LEVEL_INFO;
    } else if(0 == _logLevel.compare("WARN")) {
        return LOG_LEVEL_WARN;
    } else if(0 == _logLevel.compare("ERROR")) {
        return LOG_LEVEL_ERROR;
    } else {
        return LOG_LEVEL_DEBUG; // 如果出错，则默认级别为DEBUG
    }
}

void ConfigParser::defaultOutput(const std::string &path) {
    std::stringstream pid;
    pid << getpid();
    std::string defaultOutput = std::string("stderr") + "." + pid.str();
    freopen(defaultOutput.c_str(), "w", stderr);
    fprintf(stderr, "open path error, path is [%s] \n", path.c_str());
    fclose(stderr);
}

DLOG_END_NAMESPACE(config);
