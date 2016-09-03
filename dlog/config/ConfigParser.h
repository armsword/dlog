#ifndef DLOG_CONFIGPARSER_H
#define DLOG_CONFIGPARSER_H

#include <dlog/common/Common.h>
#include <jsoncpp/json/json.h>
#include <sstream>

DLOG_BEGIN_NAMESPACE(config);

class ConfigParser
{
public:
    ConfigParser();
    ~ConfigParser();
private:
    ConfigParser(const ConfigParser &);
    ConfigParser& operator=(const ConfigParser &);

public:
    ErrorCode parse(const std::string &path);
    const std::string& getLogPath() const;
    const std::string& getLogPrefix() const;
    uint32_t getMaxFileSize() const;
    bool getAsyncFlush() const;
    LogLevel getLogLevel();
    static void defaultOutput(const std::string &path);
    
private:
    std::string _logPath;
    std::string _logPrefix;
    std::string _logLevel;
    uint32_t _maxFileSize;
    bool _asyncFlush;
};

DLOG_TYPEDEF_PTR(ConfigParser);

DLOG_END_NAMESPACE(config);

#endif //DLOG_CONFIGPARSER_H
