#pragma  once
#ifndef __SNOWLAKE_ACCEPTOR_H__
#define __SNOWLAKE_ACCEPTOR_H__


#include "InetAddress.h"
#include "Socket.h"

namespace  snowlake{

class Acceptor
{
public:
    Acceptor(unsigned short port);
    Acceptor(const string &ip, unsigned short port);
    ~Acceptor();
    
    void ready();
    int accept();
    int fd() const;
private:
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void bind();
    void listen();
    void listen(int num);
private:
    InetAddress _addr;
    Socket _listensock;
};

};// end of namespace snowlake
#endif
