#pragma once

#ifndef __SNOWLAKE_CONDITION_H__
#define __SNOWLAKE_CONDITION_H__

#include "Noncopyable.h"
#include <pthread.h>

namespace snowlake{

class MutextLock;//类的前向声明 可以减少头文件依赖

class Condition
    : Noncopyable
{
public:
    Condition(MutextLock &mutex);
    ~Condition();
    
    void wait();
    void notify();
    void notify_all();
private:
    pthread_cond_t _cond;
    MutextLock &_mutex;
};

};// end of namespace snowlake
#endif
