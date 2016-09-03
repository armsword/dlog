#include "LoopThread.h"
#include "FileUtil.h"
#include "TimeUtility.h"
#include <unistd.h>
#include <sstream>

DLOG_BEGIN_NAMESPACE(common);

LoopThread::LoopThread() { 
    _run = false;
    _loopInterval = 0;
    _nextTime = 0;
}

LoopThread::~LoopThread() { 
    stop();
}

LoopThreadPtr LoopThread::createLoopThread(const std::tr1::function<void ()> &loopFunction, int64_t loopInterval /*us*/) 
{
    LoopThreadPtr ret(new LoopThread);
    
    ret->_loopFunction = loopFunction;
    ret->_loopInterval = loopInterval;
    ret->_run = true;
    
    ret->_threadPtr = Thread::createThread(std::bind(&LoopThread::loop, ret.get()));    
    
    if (!ret->_threadPtr) {
        std::stringstream pid;
        pid << getpid();
        std::string defaultOutput = std::string("stderr") + "." + pid.str();
        freopen(defaultOutput.c_str(), "w", stderr);
        fprintf(stderr, "create loop thread fail! \n");
        fclose(stderr);
        return LoopThreadPtr();
    }

    return ret;
}

void LoopThread::stop() {
    {
        ScopedLock lock(_cond);
        if (!_threadPtr) {
            return;
        }
        
        _run = false;
        _cond.signal();
    }
    
    _threadPtr->join();
    _threadPtr.reset();
}

void LoopThread::runOnce()
{
    ScopedLock lock(_cond);
    _nextTime = -1;
    _cond.signal();
    
}

void LoopThread::loop() {
    while (true) {
        {
            ScopedLock lock(_cond);
            if (!_run) {return;}
            int64_t nowTime = TimeUtility::currentTime();
            if (nowTime < _nextTime) {
                _cond.wait(_nextTime - nowTime);
                if (!_run) {return;}
            } 
            
            if (_nextTime == -1) {
                _nextTime = 0;
            }
            
            nowTime = TimeUtility::currentTime();
            if (nowTime < _nextTime) {
                continue;
            }
        }

        _loopFunction();

        {
            ScopedLock lock(_cond);
            if (_nextTime != -1) {
                _nextTime = TimeUtility::currentTime() + _loopInterval;
            }
        }
    }
}

DLOG_END_NAMESPACE(common);
