#pragma once
#ifndef __SNOWLAKE_TCPCONNECTION_H__
#define __SNOWLAKE_TCPCONNECTION_H__

#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"
#include "Noncopyable.h"

#include <string>
#include <memory>
#include <functional>
#include <vector>

using std::string;
using std::function;

namespace snowlake {

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr&)>;

class EventLoop;

class TcpConnection
: Noncopyable
, public std::enable_shared_from_this<TcpConnection>
{
    typedef  struct train {
        int DataLen;
        char Buf[65535];
    }train_t;

public:
    TcpConnection(int, EventLoop*);
    ~TcpConnection();
    
    std::vector<char>  receive();
    void send(const string &msg);
    void send_in_loop(const string &msg);
    string to_string()const;
    void shut_down();
    void set_connectionCallback(const TcpConnectionCallback &cb);
    void set_messageCallback(const TcpConnectionCallback &cb);
    void set_closeCallback(const TcpConnectionCallback &cb);
    void handle_connectionCallback();
    void handle_messageCallback();
    void handle_closeCallback();
private:
    InetAddress get_local_addr();
    InetAddress get_peer_addr();
private:
    Socket _sock;
    SocketIO _socketIo;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutDownWrite;
    EventLoop *_loop;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};
}// end of namespace snowlake
#endif
