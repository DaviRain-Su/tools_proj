
#include <sys/types.h>
#include <sys/socket.h>


#include "../inc/Acceptor.h"

namespace snowlake {//start of namespace snowlake 
    
Acceptor::Acceptor(unsigned short port)
    : _addr(port)
    , _listensock()
{}

Acceptor::Acceptor(const string &ip, unsigned short port)
    :_addr(ip, port)
    ,_listensock()
{}

void Acceptor::ready(){
    set_reuse_addr(true);
    set_reuse_port(true);
    bind();
    listen();
}

void Acceptor::set_reuse_addr(bool on){
    int one = on;
    if( setsockopt(_listensock.fd(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))  < 0 ){
        perror("setsockopt");
    }
}

void Acceptor::set_reuse_port(bool on){
    int one = on;
    if( setsockopt(_listensock.fd(), SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)) < 0){
        perror("setsockopt");
    }
}

void Acceptor::bind(){
    int ret = ::bind(_listensock.fd(), (struct sockaddr*)_addr.get_inet_addr_ptr(), 
                     sizeof(struct sockaddr));
    if( -1 == ret ){
        perror("bind");
    }
}

void Acceptor::listen() {
    int ret = ::listen(_listensock.fd(), 10);
    if( -1 == ret ){
        perror("listen");
    }
}

int Acceptor::accept() {
    return accept(nullptr, nullptr);
}

int Acceptor::accept(sockaddr * addr, socklen_t *addrlen){
    int peerfd = ::accept(_listensock.fd(), addr, addrlen);
    if(-1 == peerfd){
        perror("accept");
    }
    return peerfd;
}

}// end of namespace snowlake
