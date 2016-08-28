#include <dlog/config/ConfigParser.h>
#include <dlog/common/FileUtil.h>
#include <unistd.h>
#include <sstream>

DLOG_BEGIN_NAMESPACE(config);

ConfigParser::ConfigParser() { 
}

ConfigParser::~ConfigParser() { 
}

ErrorCode ConfigParser::parse(const std::string &path) {
    std::string defaultOutput = getDefaultOutput();
    freopen(defaultOutput.c_str(), "w", stderr);
    Json::Reader reader;
    Json::Value root;
    std::string content;
    bool succ = common::FileUtil::readFileContent(path,content);
    if(!succ) {
        fprintf(stderr, "open file[%s] error", path.c_str());
        return ERROR_FILE_OPEN;
    }
    if(!reader.parse(content,root)) {
        fprintf(stderr, "parse json file[%s] error", path.c_str());
        return ERROR_JSON_PARSE;
    }
    _logPath = root["log_path"].asString();
    _logPrefix = root["log_prefix"].asString();
    _maxFileSize = root["max_file_size"].asUInt();
    _asyncFlush = root["async_flush"].asBool();
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

const std::string ConfigParser::getDefaultOutput() const {
    std::stringstream pid;
    pid << getpid();
    std::string defaultOutput = std::string("stderr") + "." + pid.str();
    return defaultOutput;
}

DLOG_END_NAMESPACE(config);
