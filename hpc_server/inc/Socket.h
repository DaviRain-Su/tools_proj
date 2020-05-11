#pragma once

#ifndef __SNOWLAKE_SOCKET_H__
#define __SNOWLAKE_SOCKET_H__

namespace snowlake {// namespace snowlake

class Socket
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();

    int fd() const;
    void shut_down_write();

private:
    int _fd;
};





};// end of namespace snowlake
#endif
