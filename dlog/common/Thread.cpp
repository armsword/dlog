#include <memory>
#include "Thread.h"
#include <unistd.h>
#include <sstream>

using namespace std;

DLOG_BEGIN_NAMESPACE(common);

struct ThreadFunctionWrapperArg
{
    std::function<void ()> threadFunction;
};

void* Thread::threadWrapperFunction(void* arg)
{
    auto_ptr<ThreadFunctionWrapperArg> p(static_cast<ThreadFunctionWrapperArg*>(arg));
    p->threadFunction();
    return NULL;
}

ThreadPtr Thread::createThread(const std::function<void ()>& threadFunction)
{
    ThreadPtr threadPtr(new Thread);
    ThreadFunctionWrapperArg* arg = new ThreadFunctionWrapperArg;
    arg->threadFunction = threadFunction;
    
    int rtn = pthread_create(&threadPtr->_id, NULL,
                             &Thread::threadWrapperFunction, arg);
    
    if (rtn != 0) {
        delete arg;
        threadPtr->_id = 0;
        threadPtr.reset();
        std::stringstream pid;
        pid << getpid();
        std::string defaultOutput = std::string("stderr") + "." + pid.str();
        freopen(defaultOutput.c_str(), "w", stderr);
        fprintf(stderr, "Create thread error [%d] \n", rtn);
        fclose(stderr);
        //UTIL_LOG(FATAL, "Create thread error [%d]", rtn);
    }
    
    return threadPtr;
}

DLOG_END_NAMESPACE(common);
