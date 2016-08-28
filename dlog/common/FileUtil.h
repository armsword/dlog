#ifndef DLOG_FILEUTIL_H
#define DLOG_FILEUTIL_H

#include <dlog/common/Common.h>

DLOG_BEGIN_NAMESPACE(common);

class FileUtil
{
public:
    FileUtil();
    ~FileUtil();
private:
    FileUtil(const FileUtil &);
    FileUtil& operator=(const FileUtil &);
public:
    static bool readFileContent(const std::string &filePath, std::string &content);
private:
    
};

DLOG_TYPEDEF_PTR(FileUtil);

DLOG_END_NAMESPACE(common);

#endif //DLOG_FILEUTIL_H
