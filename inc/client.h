#pragma once

#include <iostream>
#include <time.h>
#include <list>
#include <functional>

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>

#include "Frame_head.h"
#include "utils.h"
#include "Protocol.h"

using std::list;
using std::function;

namespace snowlake { // start snowlake  

struct Client_info
{
    int _fd;//socket file discription
    unsigned char _rxbuf[FRAME_MAX_LEN]; // 接收缓冲区长度
    int _rxlen;// 当前接收数据长度
    time_t _rxtime; // 上次接收包时刻
    int _fsmstate;//状态
    static int _index; // index
    
    Client_info();
    Client_info(int fd, unsigned char *rxbuf, int rxlen, time_t rxtime,int fsmstate);
    ~Client_info();
    
    void set_client_info_fd(int fd) ;
    void set_client_info_len(int len) ;
    void set_client_info_time(time_t t);
    void set_client_info_state(int state);
};
//---------------------------------------------------------------------------------------
class client
{
public:
//    typedef int(*Fn)(Client_info &rhs,void *args);
    using Callback = std::function<int(Client_info &lhs, void *args)>;

    client();
    ~client();

    void clients_itor(Callback fn, void *args);
    void clients_itor_add_to_set(void *args);
    void clients_itor_read_process(void *args);

    int client_add_to_set(Client_info &pinfo, void *args);
    int client_read_process(Client_info &pinfo, void *args);
    void client_info_free();
    void signalHandler(int sign);

    void client_insert_info(Client_info &&lhs){
        m_clients.insert(m_clients.end(), lhs);
    }
private:
    list<Client_info> m_clients;    
    shared_ptr<Protocol> ptr_protocol;
};



};// end of namespace snowlake

