#include "../inc/Threadpool.h"
#include "../inc/Thread.h"

#include <time.h>
#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;

namespace snowlake {

Threadpool::Threadpool(size_t threadNum, size_t queSize)
    : _threadNum(threadNum)
    , _queSize(queSize)
    , _taskQue(queSize)
    , _isExit(false)
{
    _threads.reserve(_threadNum);
}

Threadpool::~Threadpool()
{
    if(!_isExit)
        stop();
}

void Threadpool::start()
{
    for(size_t idx = 0; idx != _threadNum; ++idx){
        unique_ptr<Thread> thread(new Thread( std::bind(&Threadpool::thread_func, this)));

        _threads.push_back(std::move(thread));
    }
    // start thread
    for(auto &thread : _threads){
        thread->start();
    }
}

void Threadpool::stop()
{
    if(!_isExit){
        while(!_taskQue.empty()){
            ::sleep(1);
        }
        _isExit = true;

        _taskQue.wake_up();

        for(auto & thread : _threads){
            thread->join();
        }
    }
}

void Threadpool::add_task(Task && task)
{
    _taskQue.push(task);
} 

Task Threadpool::get_task()
{
    return _taskQue.pop();
}

//每一个子线程要完成的任务，其运行在
//workerthread的run 方法中
void Threadpool::thread_func()
{
    while(!_isExit){
        Task task = get_task();
        //当任务执行的速度过快，
        //在还没有将_isExit设置为true之前，每一个子线程已经
        //阻塞在get_task()之上
        if(task)
            task();
    }
}

};// end of namespace snowlake
