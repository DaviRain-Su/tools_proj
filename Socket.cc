#include "Socket.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace snowlake{ // start namespace snowlake

Socket::Socket(){
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == _fd){
        perror("socket");
    }
}
Socket::Socket(int fd): _fd(fd) {}

int Socket::fd() const { return _fd; }

void Socket::shut_down_write(){
    ::shutdown(_fd, SHUT_WR);
}

Socket::~Socket(){
    ::close(_fd);
}

};// end of namespace std
