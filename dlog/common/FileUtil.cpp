#include <dlog/common/FileUtil.h>
#include <fstream>
#include <sstream>

DLOG_BEGIN_NAMESPACE(common);

FileUtil::FileUtil() { 
}

FileUtil::~FileUtil() { 
}

bool FileUtil::readFileContent(const std::string &filePath, std::string &content) {
    std::ifstream fileContent(filePath);
    if(!fileContent) {
        return false;
    }
    std::ostringstream buffer;
    buffer << fileContent.rdbuf();
    content = buffer.str();
    fileContent.close();
    return true;
}

DLOG_END_NAMESPACE(common);
