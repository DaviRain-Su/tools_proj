#pragma once

#ifndef  __SNOWLAKE_TASKQUEUE_H__
#define  __SNOWLAKE_TASKQUEUE_H__

#include "MutextLock.h"
#include "Condition.h"

#include <iostream>
#include <queue>
#include <functional>

using std::queue;
using std::cout;
using std::endl;

namespace  snowlake{ // namespace snowlake

using Task = std::function<void()>;
using ElemType = Task;

class TaskQueue
{
public:
    TaskQueue(size_t que_size = 10);
    ~TaskQueue() {}
    
    bool empty() const;
    bool full() const;
    void push(ElemType number);
    ElemType pop();

    void wake_up();
private:
    size_t _que_size;
    queue<ElemType> _que;
    MutextLock _mutex;
    Condition _notFull;
    Condition _notEmpty;
    bool flag;//标志的是？
};


};// end of namespace snowlake
#endif
