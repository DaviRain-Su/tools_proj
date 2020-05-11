#include "../inc/TcpServer.h"

namespace  snowlake{

TcpServer::TcpServer(const string &ip, unsigned short  port)
    : _acceptor(ip, port)
    , _loop(_acceptor)
{}


void TcpServer::start()
{
    _acceptor.ready();
    _loop.loop();
}

void TcpServer::set_connectionCallback(TcpConnectionCallback && cb)
{
    _loop.setConnectionCallback(std::move(cb));
}

void TcpServer::set_MessageCallbacl(TcpConnectionCallback && cb)
{
    _loop.setMessageCallback(std::move(cb));
}

void TcpServer::set_CloseCallback(TcpConnectionCallback && cb)
{
    _loop.setCloseCallback(std::move(cb));
}

};//end of namespace snowlake
