#include "../inc/SocketIO.h"
#include "../inc/utils.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
namespace  snowlake {

SocketIO::SocketIO(int fd)
    : _fd(fd)
{}

int SocketIO::readn(char *buff, int len)
{
    int left = len;
    char *p = buff;
    while(0 < left){
        DEBUG("left is %d\n", left);
        int ret  = ::read(_fd, p, left);
        DEBUG("ret is %d\n", ret);
        if(-1 == ret && errno == EINTR){
        DEBUG("left is %d\n", left);
            continue;
        }else if(-1 == ret){
        DEBUG("left is %d\n", left);
            perror("read");
            return len - left;
        }else if(0 == ret){
        DEBUG("left is %d\n", left);
            return  len - left;
        }else {
        DEBUG("left is %d\n", left);
            left -= ret;
            p += ret;
        }
    }
    return len - left;
}

//每次读取一行
int SocketIO::read_line(char* buff, int maxlen)
{
    int left = maxlen - 1;
    char * p = buff;
    int ret;
    int total = 0;
    while(0 < left){
        ret = recv_peek(p, left);
        //查找'\n'
        for(int idx = 0; idx != ret; ++idx){
            if(p[idx] == '\n'){
                int sz = idx + 1;
                readn(p, sz);
                total += sz;
                p += sz;
                *p = '\0';
                return total;
            }
        }
        //如果没有发现'\n'
        readn(p, ret);
        left -= ret;
        p += ret;
        total += ret;
    }
    *p = '\0';// 最终没有发现'\n'
    return total;
}


int SocketIO::recv_peek(char * buff, int len)
{
    int ret;
    do {
        ret = ::recv(_fd, buff, len, MSG_PEEK);
    }while(ret == -1 && errno == EINTR);
    return  ret;
}

int SocketIO::writen(const char* buff, int len)
{
    int left = len;
    const char *p = buff;
    while(0 < left){
        int ret = ::write(_fd, p, left);
        if(ret == -1 && errno == EINTR)
            continue;
        else if(ret == -1){
            perror("write");
            return len - left;
        }else {
            left -= ret;
            p += ret;
        }
    }
    return len - left;
}
};// end of namespace snowlake
