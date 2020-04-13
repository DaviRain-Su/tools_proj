#pragma once

namespace snowlake { // start of namespace snowlake

class Socket
{
public:
    Socket();
    explicit Socket(int fd);
    ~Socket();
    
    void shut_down_write();
    int fd() const;

private:
    int _fd;
};

}; // end of namespace snowlake

