#pragma once

#ifndef  _SNOWLAKE_SOCKETIO_H__
#define  _SNOWLAKE_SOCKETIO_H__

namespace  snowlake {

class SocketIO
{
public:
    explicit
    SocketIO(int fd);
    ~SocketIO() {}

    int readn(char* buff, int len);
    int read_line(char* buff, int maxlen);
    int writen(const char* buff, int len);
private:
    int recv_peek(char* buff, int len);
private:
    int _fd;

};
}// end of namespace snowlake
#endif
