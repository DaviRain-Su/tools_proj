#pragma once

#ifndef  __SNOWLAKE_EVENTLOOP_H__
#define __SNOWLAKE_EVENTLOOP_H__

#include "MutextLock.h"

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <vector>
#include <functional>
using std::vector;
using std::map;
using std::shared_ptr;

namespace  snowlake {

class Acceptor;
class TcpConnection;

class EventLoop
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TcpConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
    using Functor = std::function<void()>;

    EventLoop(Acceptor & acceptor);
    ~EventLoop(){}

    void loop();
    void unloop();

    void run_in_loop(Functor && cb);

    void setConnectionCallback(TcpConnectionCallback && cb)
    {
        _onConnection = std::move(cb);
    }

    void setMessageCallback(TcpConnectionCallback &&cb)
    {
        _onMeaasge = std::move(cb);
    }
    
    void setCloseCallback(TcpConnectionCallback &&cb)
    {
        _onClose = std::move(cb);
    }
private:
    void wait_epollfd();
    void handle_new_connection();
    void handle_message(int fd);
    void hand_read();

    void wakeup();
    int create_epoll_fd();
    int create_event_fd();
    void do_pending_functors();
    
    void add_epollfd_read(int fd);
    void del_epollfd_read(int fd);
    bool is_connection_closed(int fd);
private:

    int _efd;
    int _eventfd;
    Acceptor & _acceptor;
    vector<struct epoll_event> _eventList;
    map<int, TcpConnectionPtr> _conns;
    bool _isLooping;
    
    MutextLock _mutex;
    vector<Functor> _pendingFunctors;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMeaasge;
    TcpConnectionCallback _onClose;
};

};//end of namespace snowlake
#endif

