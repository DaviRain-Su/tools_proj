#pragma once

#ifndef __SNOWLAKE_INETADDRESS_H__
#define __SNOWLAKE_INETADDRESS_H__

#include <arpa/inet.h>
#include <string>
using std::string;

namespace  snowlake { // namespace snowlake

class InetAddress
{
public:
    explicit
    InetAddress(unsigned short port);
    InetAddress(const string &ip, unsigned short port);
    InetAddress(const struct sockaddr_in &addr);
    ~InetAddress();

    string ip() const;
    unsigned short port() const;
    struct sockaddr_in *get_inet_address_ptr();

private:
    struct sockaddr_in _addr;
};

};// end of namespace snowlake
#endif
