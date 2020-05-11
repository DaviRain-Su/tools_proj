#include "../inc/EventLoop.h"
#include "../inc/Acceptor.h"
#include "../inc/TcpConnection.h"

#include <unistd.h>
#include <assert.h>
#include <sys/eventfd.h>

#include <iostream>
using std::cout;
using std::endl;

namespace  snowlake {

//
//EventLoop(Acceptor &acceptor)
//
EventLoop::EventLoop(Acceptor &acceptor)
    : _efd(create_epoll_fd())
    , _eventfd(create_event_fd())
    , _acceptor(acceptor)
    , _eventList(1024)
    , _isLooping(false)
{
    add_epollfd_read(_acceptor.fd());
    add_epollfd_read(_eventfd);
}

// 
// loop()
//
void EventLoop::loop(){
    _isLooping = true;
    while(_isLooping){
        wait_epollfd();
    }
}

//
//unloop()
//
void EventLoop::unloop(){
    if(_isLooping){
        _isLooping = false;
    }
}

//
//run in loop()
//
void EventLoop::run_in_loop(Functor && cb){
    {
        MutexLockGuard autolock(_mutex);
        _pendingFunctors.push_back(std::move(cb));
    }
    wakeup();
}

//
//wait epollfd
//
void EventLoop::wait_epollfd(){
    int nready;
    do{
        nready = epoll_wait(_efd, &*_eventList.begin(), _eventList.size(), 5000);
    }while( -1 == nready &&  EINTR == errno);

    if(-1 == nready){
        perror("epoll_wait");
        return;
    }else if(0 == nready){
        cout << ">> epoll_wait timeout!" << endl;
    }else{
        if(nready == _eventList.size()){
            _eventList.resize(2 * nready);
        }

        for(int idx = 0; idx != nready; ++idx){
            int fd = _eventList[idx].data.fd;
            if(fd == _acceptor.fd()){
                //处理新连接
                if(_eventList[idx].events & EPOLLIN){
                    handle_new_connection();
                }
            }else if(fd == _eventfd){
                if(_eventList[idx].events & EPOLLIN){
                    hand_read();
                    cout << ">> before do_pending_funtors()" << endl;
                    do_pending_functors();
                    cout << ">> after do_pending_functors()" << endl;
                }
            }else{
                if(_eventList[idx].events & EPOLLIN){
                    handle_message(fd);
                }
            }
        }
    }
}

//
//handle_new_connection()
//
void EventLoop::handle_new_connection(){
    int peerfd = _acceptor.accept();
    add_epollfd_read(peerfd);
    TcpConnectionPtr conn(new TcpConnection(peerfd, this));
    conn->set_connectionCallback(_onConnection);
    conn->set_messageCallback(_onMeaasge);
    conn->set_closeCallback(_onClose);

    _conns.insert(std::make_pair(peerfd, conn));
    conn->handle_connectionCallback();
}

//
//handle_new_connection()
//
void EventLoop::handle_message(int fd){
    bool isClosed = is_connection_closed(fd);
    auto iter = _conns.find(fd);
    assert(iter != _conns.end());
    
    if(!isClosed){
        iter->second->handle_messageCallback();
    }else{
        del_epollfd_read(fd);
        iter->second->handle_closeCallback();
        _conns.erase(iter);
    }
}

//
//hand_read
//
void EventLoop::hand_read(){
    uint64_t howmany;
    int ret = ::read(_eventfd, &howmany, sizeof(howmany));
    if(ret != sizeof(howmany)){
        perror("read");
    }
}

//
//wake up
//
void EventLoop::wakeup(){
    uint64_t one = 1;
    int ret = ::write(_eventfd, &one, sizeof(one));
    if(ret != sizeof(one)){
        perror("write");
    }
}

//
//do pending functor
//
void EventLoop::do_pending_functors(){
    vector<Functor> tmp;
    {
        MutexLockGuard autolock(_mutex);
        tmp.swap(_pendingFunctors);
    }

    for(auto &functor : tmp){
        functor();
    }
}

//
//is connection closed
//
bool EventLoop::is_connection_closed(int fd){
    int ret;
    do{
        char buff[1024];
        ret = recv(fd, buff, sizeof(buff), MSG_PEEK);
    }while(ret == -1 && errno == EINTR);

    return (ret == 0);// 判断是收到的字节数
}

//
//create epoll fd
//
int EventLoop::create_epoll_fd()
{
    int ret = ::epoll_create1(0);
    if(ret == -1){
        perror("epoll_create1");
    }
    return ret;
}

//
//create event fd
//
int EventLoop::create_event_fd() {
    int ret = ::eventfd(0, 0);
    if(ret == -1){
        perror("eventfd");
    }
    return ret;
} 

// 
// add epollfd read
//
void EventLoop::add_epollfd_read(int fd){
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;
    int ret = epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &evt);
    if ( -1 == ret ){
        perror("epoll_ctl");
    }
}

// 
// del epollfd read
// 
void EventLoop::del_epollfd_read(int fd){
    struct epoll_event evt;
    evt.data.fd = fd;
    int ret = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, &evt);
    if(-1 == ret){
        perror("epoll_ctl");
    }
}

};// end of namespace snowlake
