#include "../inc/TcpConnection.h"
#include "../inc/InetAddress.h"
#include "../inc/EventLoop.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <vector>
using std::vector;

#include <sstream>
#include <iostream>
using namespace std;

namespace  snowlake {

TcpConnection::TcpConnection(int fd, EventLoop* loop)
    : _sock(fd)
    , _socketIo(fd)
    , _localAddr(get_local_addr())
    , _peerAddr(get_peer_addr())
    , _isShutDownWrite(false)
    , _loop(loop)
{}

TcpConnection::~TcpConnection()
{
    if(!_isShutDownWrite){
        shut_down();
    }
    cout << "~TcpConnection()" << endl;
}

vector<char>  TcpConnection::receive()
{
    cout << "**********************" << endl;
    // small train recv data
    int recvDatalen;//recv package len
    int ret = _socketIo.readn((char*)&recvDatalen, 4);
    if(-1 == ret){
        if(errno == EINTR){
            perror("readn");
        }
    }

    cout << ">> recv train size is " << recvDatalen << endl;

    // recv package data
    char buff[65535] = {0};// save data
    ret = _socketIo.readn(buff, recvDatalen);
    if(-1 == ret){
        if(errno == EINTR){
            perror("readn");
        }
    }
    
    vector<char> result;
    for(int i = 0; i < recvDatalen; i++){
        result.push_back(buff[i]);
    }

    cout << ">> recv result size is " << result.size() << endl;
    
    return result;
}

void TcpConnection::send(const string &msg)
{
    //data send 
    train_t train;
    train.DataLen = msg.size();
    //send data
    memcpy(train.Buf, msg.c_str(), msg.size());

    _socketIo.writen((char*)&train, msg.size() + sizeof(int));
}

void TcpConnection::send_in_loop(const string &msg)
{
    //由事件Eventloop响应发送
    _loop->run_in_loop(std::bind(&TcpConnection::send, this, msg));
}

void TcpConnection::shut_down()
{
    if(!_isShutDownWrite){
        _isShutDownWrite = true;
        _sock.shut_down_write();
    }
}


string TcpConnection::to_string() const
{
    std::ostringstream oss;
    oss << _localAddr.ip() << ": " << _localAddr.port() << "-->"
        << _peerAddr.ip() << ": " << _peerAddr.port();
    return oss.str();
}

InetAddress TcpConnection::get_local_addr() 
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if(getsockname(_sock.fd(), (struct sockaddr*)&addr, &len) == -1){
        perror("getsockname");
    }
    return  InetAddress(addr);
}

InetAddress TcpConnection::get_peer_addr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    if(getpeername(_sock.fd(), (struct sockaddr*)&addr, &len) == -1){
        perror("getpeername");
    }
    return InetAddress(addr);
}

void TcpConnection::set_connectionCallback(const TcpConnectionCallback &cb)
{
    _onConnection = std::move(cb);
}

void TcpConnection::set_messageCallback(const TcpConnectionCallback &cb)
{
    _onMessage = std::move(cb);
}

void TcpConnection::set_closeCallback(const TcpConnectionCallback &cb)
{
    _onClose = std::move(cb);
}

void TcpConnection::handle_connectionCallback()
{
    if(_onConnection){
        _onConnection(shared_from_this());
    }
}

void TcpConnection::handle_messageCallback()
{
    if(_onMessage){
        _onMessage(shared_from_this());
    }
}

void TcpConnection::handle_closeCallback()
{
    if(_onClose){
        _onClose(shared_from_this());
    }
}
};//end of namespace snowlake
