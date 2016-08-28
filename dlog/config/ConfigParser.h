#ifndef DLOG_CONFIGPARSER_H
#define DLOG_CONFIGPARSER_H

#include <dlog/common/Common.h>
#include <jsoncpp/json/json.h>

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

private:
    const std::string getDefaultOutput() const;
    
private:
    std::string _logPath;
    std::string _logPrefix;
    uint32_t _maxFileSize;
    bool _asyncFlush;
};

DLOG_TYPEDEF_PTR(ConfigParser);

DLOG_END_NAMESPACE(config);

#endif //DLOG_CONFIGPARSER_H
