#pragma once

#include <arpa/inet.h>
#include <string>

using std::string;

namespace snowlake { // start namespace snowlake

class InetAddress
{
public:
    explicit
    InetAddress(unsigned short port);
    InetAddress(const string &ip, unsigned short port);
    InetAddress(const struct sockaddr_in &addr);
    ~InetAddress() {}

    string ip() const;
    unsigned short port() const;
    struct sockaddr_in * get_inet_addr_ptr(){ return &_addr; }
private:
    struct sockaddr_in _addr;
};

}// end of namespace snowlake
