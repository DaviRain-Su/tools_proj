#pragma once

#ifndef  __SNOWLAKE_THREADPOOL_H__
#define __SNOWLAKE_THREADPOOL_H__
#include "TaskQueue.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

namespace  snowlake{

class Thread;

class Threadpool
{
public:
    Threadpool(size_t threadNum = 4, size_t queSize = 10);
    ~Threadpool();
    
    void start();
    void add_task(Task && task);
    void stop();
private:
    Task get_task();
    void thread_func();
private:
    size_t _threadNum;
    size_t _queSize;
    vector<unique_ptr<Thread>> _threads;
    TaskQueue _taskQue;
    bool _isExit;//判断线程池什么时候退出的标记
};

};// end of namespace snowlake
#endif
