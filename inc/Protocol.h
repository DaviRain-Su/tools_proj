#pragma once

#include <iostream>
#include <list>
#include <memory>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "Frame_head.h"
#include "utils.h"
#include "ps_process.h"
#include "uio.h"

using std::list;
using std::cout;
using std::endl;
using std::shared_ptr;

namespace  snowlake { // start namespace snowlake

class Protocol
{

    struct Work_info {
        Input_info_t input_info;// 图像信息
        
        uint64_t ann_busy; 
        
        void set_init_info( Input_info_t &lhs){
            input_info = lhs;
        }
        //ctr
        Work_info() {
            std::cout << "Work_info()" << std::endl;
        }
        ~Work_info() {
            std::cout << "~Work_info()" << std::endl;
        }
    };

public:
    Protocol();
    ~Protocol();

    int cmd_process(int socket, unsigned char* cmdbuf, unsigned int len);

private:
    //这里有问题
    int input_data_check(Input_info_t &lhs);
    int init_data_check(Init_info_t &lhs);
    int output_data_check(Output_info_t &lhs);

    int simple_cmd_reply(int socket,  uint16_t seq, unsigned char cmd, int8_t rtcode);
    int simple_cmd_reply_data(int socket, uint16_t seq, unsigned char cmd, int8_t rtcode, void* databuf, unsigned int datalen);
    int cmd_begin_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_tx_data_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_calc_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_config_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_get_data_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_status_reply(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_test_reg_reply(int socket, unsigned char *cmdbuf, unsigned int len);

    int feed_dog();

private:
    static uint32_t OUT_PUT_ADDRESS; // 当前输入参数大小
    static uint32_t DATA_READ_ADDRESS_OFFSET; // 当前输出数据大小
    
    Work_info work_info;    
    shared_ptr<Reg> ptr_reg;
    shared_ptr<PS_process> ptr_ps;
};

};// end of namespace snowlake
