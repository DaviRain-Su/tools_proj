#include "../inc/Thread.h"

#include <iostream>
using std::cout;
using std::endl;

namespace  snowlake{

Thread::Thread(ThreadCallback &&cb)
    :_pthid(0)
    ,_is_running(false)
    ,_cb(std::move(cb))
{}

void Thread::start()
{
    pthread_create(&_pthid, nullptr, threadfunc, this);
    _is_running = true;
}

void* Thread::threadfunc(void *arg)
{
    Thread* pthread = static_cast<Thread*>(arg);
    if(pthread)
        pthread->_cb();
    return nullptr;
}

void Thread::join()
{
    if(_is_running){
        pthread_join(_pthid, nullptr);
        _is_running = false;
    }
}

Thread::~Thread()
{
    if(_is_running){
        pthread_detach(_pthid);
    }
}

}// end of namespace snowlake
