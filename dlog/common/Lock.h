#ifndef DLOG_LOCK_H
#define DLOG_LOCK_H

#include "Common.h"
#include <pthread.h>
#include "TimeUtility.h"
#include <cassert>

DLOG_BEGIN_NAMESPACE(common);

class ThreadMutex
{
public:
    ThreadMutex(const pthread_mutexattr_t *mta = NULL) {
        pthread_mutex_init(&_mutex, mta);
    }
    
    ~ThreadMutex() {
        pthread_mutex_destroy(&_mutex);
    }
    
    int lock() {
        return pthread_mutex_lock(&_mutex);
    }

    int trylock () {
        return pthread_mutex_trylock(&_mutex);
    }

    int unlock() {
        return pthread_mutex_unlock(&_mutex);
    }
private:
    ThreadMutex(const ThreadMutex&);
    ThreadMutex& operator = (const ThreadMutex&);
protected:
    pthread_mutex_t _mutex;
};

class RecursiveThreadMutex : public ThreadMutex
{
public:
    RecursiveThreadMutex() 
        : ThreadMutex(RECURSIVE_PTHREAD_MUTEXATTR_PTR)
    {}

private:
    RecursiveThreadMutex(const RecursiveThreadMutex&);
    RecursiveThreadMutex& operator = (const RecursiveThreadMutex&);
private:
    static const pthread_mutexattr_t *RECURSIVE_PTHREAD_MUTEXATTR_PTR;
};

class ThreadCond : public ThreadMutex
{
public:
    ThreadCond() {
        pthread_cond_init(&_cond, NULL);
    }

    ~ThreadCond() {
        pthread_cond_destroy(&_cond);
    }

    int wait(int64_t usec = 0)
    {
        int ret = 0; 
        if (usec <= 0) {
            ret = pthread_cond_wait(&_cond, &_mutex);
        } else {
            timespec ts = TimeUtility::getTimespec(usec);
            ret = pthread_cond_timedwait(&_cond, &_mutex, &ts);
        }
        
        return ret;
    }
    
    int signal() {
        return pthread_cond_signal(&_cond);
    }

    int broadcast() {
        return pthread_cond_broadcast(&_cond);
    }

protected:
    pthread_cond_t  _cond;
};

class ProducerConsumerCond : public ThreadMutex
{
public:
    ProducerConsumerCond() {
        pthread_cond_init(&_producerCond, NULL);
        pthread_cond_init(&_consumerCond, NULL);
    }

    ~ProducerConsumerCond() {
        pthread_cond_destroy(&_producerCond);
        pthread_cond_destroy(&_consumerCond);
    }
public:
    int producerWait(int64_t usec = 0) {
        return wait(_producerCond, usec);
    }

    int signalProducer() {
        return signal(_producerCond);
    }

    int broadcastProducer() {
        return broadcast(_producerCond);
    }

    int consumerWait(int64_t usec = 0) {
        return wait(_consumerCond, usec);
    }

    int signalConsumer() {
        return signal(_consumerCond);
    }

    int broadcastConsumer() {
        return broadcast(_consumerCond);
    }

private:
    int wait(pthread_cond_t &cond, int64_t usec)
    {
        int ret = 0; 
        if (usec <= 0) {
            ret = pthread_cond_wait(&cond, &_mutex);
        } else {
            timespec ts = TimeUtility::getTimespec(usec);
            ret = pthread_cond_timedwait(&cond, &_mutex, &ts);
        }
        
        return ret;
    }
    
    static int signal(pthread_cond_t &cond) {
        return pthread_cond_signal(&cond);
    }
    
    static int broadcast(pthread_cond_t &cond) {
        return pthread_cond_broadcast(&cond);
    }

protected:
    pthread_cond_t  _producerCond;
    pthread_cond_t  _consumerCond;
};

class ScopedLock
{
private:
    ThreadMutex &_lock;
private:
    ScopedLock(const ScopedLock &);
    ScopedLock& operator= (const ScopedLock &);
    
public:
    explicit ScopedLock(ThreadMutex& lock) : _lock(lock) {
        int ret = _lock.lock();
        assert(ret == 0); (void) ret;
    }

    ~ScopedLock() {
        int ret = _lock.unlock();
        assert(ret == 0); (void) ret;
    }
};

class ReadWriteLock
{
private:
    ReadWriteLock(const ReadWriteLock&);
    ReadWriteLock& operator = (const ReadWriteLock&);
public:
    enum Mode {
        PREFER_READER,
        PREFER_WRITER
    };

    ReadWriteLock(Mode mode = PREFER_WRITER) {
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        switch (mode)
        {
        case PREFER_WRITER:
            pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
            break;
        case PREFER_READER:
            pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_READER_NP);
            break;
        default:
            break;
        }
        pthread_rwlock_init(&_lock, &attr);
    }

    ~ReadWriteLock() {
        pthread_rwlock_destroy(&_lock);
    }

    int rdlock() {
        return pthread_rwlock_rdlock(&_lock);
    }

    int wrlock() {
        return pthread_rwlock_wrlock(&_lock);
    }

    int tryrdlock() {
        return pthread_rwlock_tryrdlock(&_lock);
    }

    int trywrlock() {
        return pthread_rwlock_trywrlock(&_lock);
    }

    int unlock() {
        return pthread_rwlock_unlock(&_lock);
    }

protected:
    pthread_rwlock_t _lock;
};

class ScopedReadLock
{
private:
    ReadWriteLock &_lock;
private:
    ScopedReadLock(const ScopedReadLock&);
    ScopedReadLock& operator = (const ScopedReadLock&);
public:
    explicit ScopedReadLock(ReadWriteLock &lock) 
        : _lock(lock) 
    {
        int ret = _lock.rdlock();
        assert(ret == 0); (void) ret;
    }
    ~ScopedReadLock() {
        int ret = _lock.unlock();
        assert(ret == 0); (void) ret;
    }
};

class ScopedWriteLock
{
private:
    ReadWriteLock &_lock;
private:
    ScopedWriteLock(const ScopedWriteLock&);
    ScopedWriteLock& operator = (const ScopedWriteLock&);
public:
    explicit ScopedWriteLock(ReadWriteLock &lock) 
        : _lock(lock) 
    {
        int ret = _lock.wrlock();
        assert(ret == 0); (void) ret;
    }
    ~ScopedWriteLock() {
        int ret = _lock.unlock();
        assert(ret == 0); (void) ret;
    }
};

class ScopedReadWriteLock
{
private:
    ReadWriteLock& _lock;
    char _mode;
private:
    ScopedReadWriteLock(const ScopedReadWriteLock&);
    ScopedReadWriteLock& operator = (const ScopedReadWriteLock&);

public:
    explicit ScopedReadWriteLock(ReadWriteLock& lock, const char mode) 
        : _lock(lock), _mode(mode)
    {
        if (_mode == 'r' || _mode == 'R') {
            int ret = _lock.rdlock();
            assert(ret == 0); (void) ret;
        } else if (_mode == 'w' || _mode == 'W') {
            int ret = _lock.wrlock();
            assert(ret == 0); (void) ret;
        }
    }
    
    ~ScopedReadWriteLock()
    {
        if (_mode == 'r' || _mode == 'R' 
            || _mode == 'w' || _mode == 'W') 
        { 
            int ret = _lock.unlock();
            assert(ret == 0); (void) ret;
        }
    }
};

class Notifier
{
public:
    static const int EXITED = 1 << (16 + 1);
public:
    Notifier() : _accumulatedNotification(0), _exitFlag(false)
    {}
    
    int notifyExit()
    {
        ScopedLock lock(_cond);
        _exitFlag = true;
        return _cond.broadcast();
    }
    
    int notify()
    {
        ScopedLock lock(_cond);
        ++_accumulatedNotification;
        return _cond.signal();
    }
    
    /**
     * return 0 if got notified, and
     * return errno or EXITED
     */
    int waitNotification(int timeout = -1)
    {
        return wait(timeout);
    }

private:
    volatile int _accumulatedNotification;
    volatile bool _exitFlag;
    ThreadCond _cond;

private:    
    int wait(int timeout) {
        ScopedLock lock(_cond);
        while (true) {
            if (_exitFlag) {
                return EXITED;
            }

            if (_accumulatedNotification > 0) {
                _accumulatedNotification--;
                return 0;
            } 
            
            int ret = _cond.wait(timeout);
            if (ret != 0) {
                return ret;
            }
        }
    }
};

class TerminateClosure
{
public:
    TerminateClosure() {}
    virtual ~TerminateClosure() {}
public:
    virtual void Run() = 0;
};

class TerminateNotifier
{
public:
    static const int EXITED = 1 << (16 + 1);
public:
    TerminateNotifier() : _count(0), _exitFlag(false)
                        ,_closure(NULL)
    {}
    /***
        dec must be called with inc in pair. 
        And dec must be called after inc.
     */
    int inc() {
        common::ScopedLock lock(_cond);
        ++_count;
        return 0;
    }
    int dec() {
        TerminateClosure * closure = NULL;
        int ret = 0;
        // get closure out of the lock range
        // to prevent "Run" delete "this"
        {
            // ENTER LOCK RANGE
            common::ScopedLock lock(_cond);
            --_count;
            assert(_count >= 0);
            if (_count == 0) {
                if (_closure) {
                    closure = _closure;
                    _closure = NULL;
                }
                ret = _cond.broadcast();
            }
            // LEAVE LOCK RANGE
        }
        
        if (closure) {
            closure->Run();
        }
        return ret;
    }
    /**
     return 0 if got notified, and
     return errno or EXITED
     ether onTerminate or wait can be called, but not together
     */
    int wait(int timeout = -1) {
        common::ScopedLock lock(_cond);
        while (true) {
            if (_count == 0) {
                return 0;
            }

            if (_exitFlag) {
                return EXITED;
            }

            int ret = _cond.wait(timeout);
            if (ret != 0) {
                return ret;
            }
        }
    }
    int peek() {
        common::ScopedLock lock(_cond);
        return _count;
    }
    /*
      After onTerminate, NO MORE inc and dec should be called.
      The closure will be call only once.
      The closure MUST be self deleted or deleted by outer caller.
      
      ether onTerminate or wait can be called, but not together
    */
    int onTerminate(TerminateClosure * closure) {
        {
            // ENTER LOCK RANGE
            common::ScopedLock lock(_cond);
            if ( (_count == 0) && (closure) ) {
                // event happen, and the new closure is not NULL
                // new closure should be called right now
                _closure = NULL;
            }
            else {
                // event not happen yet, or NULL is set
                // simply save it
                _closure = closure;
                return 0;
            }
            // LEAVE LOCK RANGE
        }
        
        if (closure) {
            closure->Run();
        }
        return 0;
    }
/*
    int notifyExit()
    {
        autil::ScopedLock lock(_cond);
        _exitFlag = true;
        // FIXME: may not alloc by new
        if (_closure) {
            delete _closure;
            _closure = NULL;
        }
        return _cond.broadcast();
    }
*/
private:
    volatile int _count;
    volatile bool _exitFlag;
    common::ThreadCond _cond;
    TerminateClosure * _closure;
};

DLOG_END_NAMESPACE(common);

#endif // DLOG_LOCK_H
