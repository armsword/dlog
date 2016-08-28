#ifndef DLOG_LOOPTHREAD_H
#define DLOG_LOOPTHREAD_H

#include "Common.h"
#include "Thread.h"
#include "Lock.h"

DLOG_BEGIN_NAMESPACE(common);

class LoopThread;
typedef std::shared_ptr<LoopThread> LoopThreadPtr;

class LoopThread
{
private:
    LoopThread();
public:
    ~LoopThread();
private:
    LoopThread(const LoopThread &);
    LoopThread& operator = (const LoopThread &);
public:
    void stop();
public:
    // if create loop thread fail, return null LoopThreadPtr
    static LoopThreadPtr createLoopThread(const std::function<void ()> &loopFunction, int64_t loopInterval /*us*/);
    void runOnce();
private:
    void loop();
private:
    volatile bool _run;
    ThreadCond _cond;
    ThreadPtr _threadPtr;
    std::function<void ()> _loopFunction;
    int64_t _loopInterval;
    int64_t _nextTime;
};

DLOG_END_NAMESPACE(common);

#endif //DLOG_LOOPTHREAD_H
