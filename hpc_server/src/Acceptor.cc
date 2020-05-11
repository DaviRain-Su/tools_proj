#include "../inc/Acceptor.h"

#include <sys/types.h>
#include <sys/socket.h>

namespace  snowlake{

//
//Acceptor(unsigned int short port)
//
Acceptor::Acceptor(unsigned short port)
    : _addr(port)
    , _listensock()
{}

//
//Acceptor(const strig &ip, unsigned short port)
//
Acceptor::Acceptor(const string& ip, unsigned short port)
    : _addr(ip, port)
    , _listensock()
{}

//
//~Acceptor()
//
Acceptor::~Acceptor(){}

//
//fd()
//
int Acceptor::fd() const {
    return _listensock.fd();
}

//
//ready()
//
void Acceptor::ready()
{
    set_reuse_addr(true);
    set_reuse_port(true);
    bind();
    listen();
}

//
//set reuse addr
//
void Acceptor::set_reuse_addr(bool on){
    int one = on;
    if(setsockopt(_listensock.fd(),
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &one,
                  sizeof(one)) < 0){
        perror("setsockopt");
    }
}

//
//set reuse port
//
void Acceptor::set_reuse_port(bool on){
    int one = on;
    if(setsockopt(_listensock.fd(),
                  SOL_SOCKET, 
                  SO_REUSEPORT, 
                  &one,
                  sizeof(one)) < 0){
        perror("setsockopt");
    }
}

//
//bind
//
void Acceptor::bind(){
    int ret = ::bind(_listensock.fd(),
                     (struct sockaddr*)_addr.get_inet_address_ptr(),
                     sizeof(struct sockaddr));
    if(-1 == ret){
        perror("bind");
    }
}

//
//listen
//
void Acceptor::listen() {
    listen(10);
}

//
//listen()
//
void Acceptor::listen(int num){
    int ret = ::listen(_listensock.fd(), num);
    if( -1 == ret ){
        perror("listen");
    }
}

//
//accept()
//
int Acceptor::accept(){
    int peerfd = ::accept(_listensock.fd(), nullptr, nullptr);
    if(-1 == peerfd){
        perror("accept");
    }
    return  peerfd;
}

}// end of namespace snowlake
