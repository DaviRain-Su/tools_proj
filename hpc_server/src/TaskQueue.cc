#include "../inc/TaskQueue.h"

#include <iostream>
using std::endl;
using std::cout;

namespace  snowlake{

TaskQueue::TaskQueue(size_t que_size)
    :_que_size(que_size)
    , _mutex()
    , _notFull(_mutex)
    , _notEmpty(_mutex)
{}

bool TaskQueue::empty() const 
{
    return _que.size() == 0;
}

bool TaskQueue::full() const 
{
    return _que.size() == _que_size;
}

void TaskQueue::push(ElemType number)
{
    {
        MutexLockGuard autolock(_mutex);

        while(full()){//使用while是为了防止出现虚假唤醒
            _notFull.wait();//当还是full 异常唤醒， 虚假唤醒
            //也就是唤醒之后没有再一次判断，queue是不是full
            //应该再一次判断是不是满的状态
        }
        _que.push(number);
    }
    _notEmpty.notify();
}


ElemType TaskQueue::pop()
{
    ElemType ret;
    {
        MutexLockGuard autolock(_mutex);
        while(flag && empty()){
            _notEmpty.wait();
        }
        if(flag){
            ret = _que.front();
            _que.pop();
            _notFull.notify();
        }else{
            return nullptr;
        }
    }
    return ret;
}

void TaskQueue::wake_up()
{
    flag = false;
    _notEmpty.notify_all();
}

};// end of namespace snowlake
