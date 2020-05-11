#pragma once
#include "Message.h"
#include "PsProcess.h"
#include "TcpConnection.h"

#include <iostream>

#ifndef  __SNOWLAKE_WORKTASK_H__
#define  __SNOWLAKE_WORKTASK_H__

namespace  snowlake
{

//
//初始化信息
//
struct init_info_t {
    uint64_t base_address;//基地址
    char interrupt_path[12];//中断文件路径
};

//
//input offset info
//

struct input_info_t {
    uint64_t input_address;//读入地址
};

//
//output offset and size info 
//

struct output_info_t {
    uint64_t output_address;//输出地址
    uint64_t output_data_size;//data大小
};

//
//reg index
//
struct reg_index_t {
    uint64_t index_value;//index
};

//
//work info 
//
struct work_info_t
{
    input_info_t input_info;
    uint64_t ann_busy;
};

class Message;
class WorkTask
{
public:
    WorkTask(const string &msg,  const TcpConnectionPtr & conn) 
        : _msg(msg),
          _conn(conn), 
          _message(),
          _uio(),
          _ps()
    {}
    ~WorkTask() {}
   
    void process();// compute msg
private:
    //compute
    Message && cmd_process(const Message &lhs);
    
    // reply
    Message && simple_cmd_reply(const Message &lhs, unsigned char cmd, uint8_t rtcode);
    Message && simple_cmd_reply_data(const Message &lhs, unsigned char cmd, uint8_t rtcode, const vector<char> &databuf);

    Message && cmd_begin_reply(const Message &lhs);
    Message && cmd_tx_data_reply(const Message &lhs);
    Message && cmd_calc_reply(const Message &lhs);
    Message && cmd_get_data_reply(const Message &lhs);
    Message && cmd_status_reply(const Message &lhs);
    Message && cmd_config_reply(const Message &lhs);
    Message && cmd_test_reg_reply(const Message &lhs);

    bool input_data_check(const init_info_t &lhs) 
    {
        return true;
    }
    
    bool input_data_check(const input_info_t &lhs) 
    {
        return true;
    }
    
    bool output_data_check(const output_info_t &lhs)
    {
        return true;
    }
private:
    static uint32_t OUT_PUT_ADDRESS;//当前输入参数大小
     
    static uint32_t DATA_READ_ADDRESS_OFFSET;//当前输出数据大小
    
    work_info_t work_info;

    string _msg; //接收到的消息
    TcpConnectionPtr _conn; //处理这一消息的这一链接
    
    //Buffer _buffer;// for encode message and decode message
    Message _message;// for send and recv message
    Uio _uio;   
    PsProcess _ps;
};
}// end of namespace snowlake
#endif
