#pragma once

#ifndef  __MUTEXTLOCK_H__
#define  __MUTEXTLOCK_H__

#include <iostream>
#include <cstdio>

#include <pthread.h>
#include <assert.h>

#include "Noncopyable.h"

#define CHECK(exp) if(!exp)\
{\
    fprintf(stderr, "File:%s, Line: %d Exp:["#exp"] is true, abort.\n", __FILE__, __LINE__ );\
    abort();\
}

namespace snowlake{

class MutextLock
    : public Noncopyable
{
    friend class Condition;//条件变量友元声明
public:
    MutextLock();
    ~MutextLock();

    void lock();
    void unlock();

    bool is_locking() const;

    pthread_mutex_t* get_mutex_ptr();
private:
    void restore_mutex_status();
private:
    pthread_mutex_t _mutex;
    bool _isLocking;
};

// RAII
class MutexLockGuard
    : public Noncopyable
{
public:
    MutexLockGuard(MutextLock &mutex)
        : _mutex(mutex)//构造时加锁
    {
        _mutex.lock();
    }
    ~MutexLockGuard()//析构时解锁
    {
        _mutex.unlock();
    }
private:
    MutextLock &_mutex;
};

};//end of namespace snowlake
#endif
