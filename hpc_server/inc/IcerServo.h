#pragma once
#ifndef  _SNOWLAKE_ICERSERVO_H__
#define  _SNOWLAKE_ICERSERVO_H__
#include "Threadpool.h"
#include "TcpServer.h"

namespace  snowlake {

class IcerServo
{
public:
    IcerServo(const string &ip, unsigned short port)
        :_threadpool(THREADNUM, QUEUESIZE),
         _tcpserver(ip, port)
    {}
    ~IcerServo() {}

    void start();   

private:
    void onConnection(TcpConnectionPtr conn);
    void onMessage(TcpConnectionPtr conn);
    void onClose(TcpConnectionPtr conn);

private:
    const static int  THREADNUM = 4;
    const static int  QUEUESIZE = 10;

    Threadpool _threadpool;
    TcpServer _tcpserver;
};

}// end of snowlake
#endif
