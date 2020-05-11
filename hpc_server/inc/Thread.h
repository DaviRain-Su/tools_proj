#pragma once

#ifndef __SNOWLAKE_THREAD_H___
#define __SNOWLAKE_THREAD_H___

#include <pthread.h>
#include <functional>

#include "Noncopyable.h"

using std::function;
namespace  snowlake {

class Thread
    : Noncopyable
{
public:
    using ThreadCallback = function<void()>;

    Thread(ThreadCallback &&cb);
    ~Thread();

    void start();
    void join();
private:
    static void *threadfunc(void*);
private:
    pthread_t _pthid;//pthred id
    bool _is_running;//判断是否在运行
    ThreadCallback _cb;
};

};// end of namespace snowlake
#endif
