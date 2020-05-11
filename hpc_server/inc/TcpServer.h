#pragma once

#ifndef  __SNOWLAKE_TCPSERVER_H__
#define  __SNOWLAKE_TCPSERVER_H__
#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

namespace  snowlake{

class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port);
    ~TcpServer(){}
    
    void start();
    void set_connectionCallback(TcpConnectionCallback && cb);
    void set_MessageCallbacl(TcpConnectionCallback && cb);
    void set_CloseCallback(TcpConnectionCallback && cb);
private:
    Acceptor _acceptor;
    EventLoop _loop;
};
};//end of namespace snowlake
#endif

