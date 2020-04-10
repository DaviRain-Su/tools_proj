#pragma once

#include "InetAddress.h"
#include "Socket.h"

namespace snowlake {


class Acceptor
{
public:
    Acceptor(unsigned short port);
    Acceptor(const string &ip, unsigned short port);
    void ready();
    int accept();
    int fd() const { 
        return _listensock.fd();
    }

    ~Acceptor();

private:
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void bind();
    void listen();
private:
    InetAddress _addr;
    Socket _listensock;

};// end of namespace snowlake
};// end of namespace snowlake
