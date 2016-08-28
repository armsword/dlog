#include "Lock.h"

DLOG_BEGIN_NAMESPACE(common);

class RecursivePthreadMutexattr {
public:
    RecursivePthreadMutexattr() {
        pthread_mutexattr_init(&_mta);
        pthread_mutexattr_settype(&_mta, PTHREAD_MUTEX_RECURSIVE);
    }
    
    ~RecursivePthreadMutexattr() {
        pthread_mutexattr_destroy(&_mta);
    }
    
public:
    pthread_mutexattr_t _mta;
};

static const RecursivePthreadMutexattr 
    sRecursivePthreadMutexattr = RecursivePthreadMutexattr();

const pthread_mutexattr_t *
RecursiveThreadMutex::RECURSIVE_PTHREAD_MUTEXATTR_PTR 
    = &sRecursivePthreadMutexattr._mta;

DLOG_END_NAMESPACE(common);
