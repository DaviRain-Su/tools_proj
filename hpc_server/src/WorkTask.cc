#include "../inc/WorkTask.h"
#include "../inc/md5.h"
#include "../inc/Buffer.h"
// codec
#include "../inc/hpc.msg.pb.h"
#include "../inc/codec.h"

#include <stdio.h> //fopen
#if 1
#include <string.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using std::stringstream;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::endl;
using std::cout;
using std::move;

namespace snowlake {

//
//static number init
//

uint32_t WorkTask::OUT_PUT_ADDRESS = 0;
uint32_t WorkTask::DATA_READ_ADDRESS_OFFSET = 0;

#if 0 // echo 
void WorkTask::process()
{
    cout << "recv this msg is " << _msg << endl;

    string reponse = _msg;

    _conn->send_in_loop(reponse);    
}
#endif

void WorkTask::process() // 这里客户端也要对应的进行 decode process encode  
{  
#if 0
     Buffer _buffer;
    string md5 = MD5(_msg).toStr();
    cout << " >> md5 is " << md5 << endl;

    // 对收到的string 解码为Message格式
    Message message = _buffer.decode(_msg);//解码
    message.to_string();

    // 对收到的数据进行处理
    Message resultMessage = cmd_process(message);//计算, 主逻辑处理
    
    Buffer enbuf(resultMessage.get_full_package_size());

    // 将处理后的数据编码
    string sendMsg = enbuf.encode(resultMessage);//编码

    cout << ">> encode size is " << sendMsg.size() << endl;
    
    // 发送最后编码的数据
    _conn->send_in_loop(sendMsg);//发送
#endif
    string md5 = MD5(_msg).toStr();
    cout << ">> md5 is " << md5 << endl;
    cout << ">>_msg size is " << _msg.size() << endl;
    
    string response = _msg;
#if 0
    int32_t be32 = 0; 
    std::copy(response.begin(), response.begin() + sizeof(be32), reinterpret_cast<char*>(&be32));
    int32_t len = ::ntohl(be32);
    assert(len == response.size() - sizeof(be32));

    std::string buf = response.substr(sizeof(int32_t));
    assert(len == buf.size());

    hpc::cmd_begin_reply * newMsg = dynamic_cast<hpc::cmd_begin_reply*>(decode(buf));
    assert(newMsg != NULL);
    newMsg->PrintDebugString();
#endif

    _conn->send_in_loop(response);
}

//
//对收到的数据按不同的命令进行不同的处理
//
Message && WorkTask::cmd_process(const Message &lhs)
{
    switch(lhs.get_cmd())
    {
        case CMD::BEGIN: 
            cout << "BEGIN" << endl;
            
            return cmd_begin_reply(lhs);
        break;
        case CMD::TX_DATA:
            cout << "TX_DATA" << endl;
            
            return cmd_tx_data_reply(lhs);
        break;
        case CMD::START_CAL:
            cout << "START_CAL" << endl;
            
            return cmd_calc_reply(lhs);
        break;
        case CMD::GET_DATA:
            cout << "GET_DATA" << endl;
            
            return cmd_get_data_reply(lhs);
        break;
        case CMD::PL_STATUS:
            cout << "PL_STATUS" << endl;
            
            return cmd_status_reply(lhs);
        break;
        case CMD::CONFIG:
            cout << "CONFIG" << endl;
            
            return cmd_config_reply(lhs);
        break;
        case CMD::TEST_REG:
            cout << "TEST_REG" << endl;
            
            return cmd_test_reg_reply(lhs);
        break;
        default: // 当收到出错的指令, 这里做一些特出的处理
            cout << ">> unknown cmd <<" << endl;
            return move(Message());// 发送一个none
        break;
    }
}

Message&& WorkTask::simple_cmd_reply(const Message &lhs, unsigned char cmd, uint8_t rtcode)
{
        // default seting preamble version flag endsym
        Message tempMsg; 
        
        //default seting preamble

        int len = 0;
    
        // for preamble size
        len += tempMsg.get_preamble_size();
       
        // for len size
        len += tempMsg.get_len_size();
        
        // for version size
        len += tempMsg.get_version_size();
        
        // for total package size
        len += tempMsg.get_total_package_len_size();
        
        // for seq size 
        len += tempMsg.get_seq_size();
        
        // for flag size
        len += tempMsg.get_flag_size();
        
        //for payload of oridinary 
        len += tempMsg.get_cmd_status_size();
        len += tempMsg.get_err_status_size();
        
        // for checksum size
        len += tempMsg.get_checksum_size();
        
        // for end sym size
        len += tempMsg.get_end_sym_size();

        //设置len
        tempMsg.set_len(len); // 设置以上的len
        
        //default setting version

        //设置total_package_len
        tempMsg.set_total_package_len(lhs.get_total_package_len());

        //设置seq
        tempMsg.set_seq(lhs.get_seq());
        
        //default setting flag
        
        //设置cmd
        //char cmd = CMD::BEGIN;
        tempMsg.set_payload(cmd);
        
        //设置 rtcode
        //char rtcode = ERR_STATUS::PARAM;
        tempMsg.set_payload(rtcode);

        //设置 checksum
        string rtMsg = tempMsg.get_Message_string(); //得到消息包的组装消息
        string retCheckSum = MD5(rtMsg).toStr();//将组装后的数据求出计算后的MD5值为32字节
        tempMsg.set_checksum(retCheckSum); // 设置md5值

        //default setting endsym
       
        return move(tempMsg);
}

Message && WorkTask::simple_cmd_reply_data(const Message &lhs, unsigned char cmd, uint8_t rtcode, const vector<char> & databuf)
{

        // default seting preamble version flag endsym
        Message tempMsg; 
        
        //default seting preamble

        int len = 0;
        
        // for pareamble size
        len += tempMsg.get_preamble_size();
        
        // for len size
        len += tempMsg.get_len_size();
        
        // for version size
        len += tempMsg.get_version_size();
        
        // for total package len size 
        len += tempMsg.get_total_package_len_size();
        
        // for seq size
        len += tempMsg.get_seq_size();
        
        // for flag size
        len += tempMsg.get_flag_size();
        
        //for payload
        //for cmd size
        len += tempMsg.get_cmd_status_size();
        
        // for err status size
        len += tempMsg.get_err_status_size();
        
        //add databuf size of extract data 
        len += databuf.size();
        
        // for checksum size
        len += tempMsg.get_checksum_size();
        
        // for end sym size
        len += tempMsg.get_end_sym_size();



        //设置len
        tempMsg.set_len(len); // 设置以上的len
        
        //default setting version

        //设置total_package_len
        tempMsg.set_total_package_len(lhs.get_total_package_len());

        //设置seq
        tempMsg.set_seq(lhs.get_seq());
        
        //default setting flag
        
        //设置cmd
        tempMsg.set_payload(cmd);
        
        //设置 rtcode
        tempMsg.set_payload(rtcode);

        //设置 databuf
        string tmp(databuf.begin(),databuf.end());
        tempMsg.set_payload(tmp);

        //设置 checksum
        string rtMsg = tempMsg.get_Message_string(); //得到消息包的组装消息
        string retCheckSum = MD5(rtMsg).toStr();//将组装后的数据求出计算后的MD5值为32字节
        tempMsg.set_checksum(retCheckSum); // 设置md5值

        //default setting endsym
       
        return move(tempMsg);
}

Message && WorkTask::cmd_begin_reply(const Message &lhs) 
{
    //begin 第一次接收到真实的带外数据进行提取后，
    //放入到 init_info
    //接受道的数据有base_address
    //和 interrupt_path 
    init_info_t init_info;
    vector<char> payload = lhs.get_real_payload(); // 从lhs 获取真正的payload 数据
    
    memcpy(&init_info, payload.data(), sizeof(init_info_t));//将获取到的数据拷贝道init_info
    
    if(!input_data_check(init_info)){ //检查收到的输入的地址的数据是否合法

        //设置 cmd rtcode
        unsigned char cmd = CMD::BEGIN;
        uint8_t rtcode = ERR_STATUS::PARAM; //设置这时的出错码
        
        return simple_cmd_reply(lhs, cmd, rtcode);//返回出出错状态和cmd命令

    }else{
        
        //get base_address
        unsigned int base_address = (unsigned int)(init_info.base_address & 0xffffffff);
        //get interrupt_path 
        char *interrupt_path = init_info.interrupt_path;

        std::cout << "base address " << base_address << std::endl;
        std::cout << "interrupt_path " << interrupt_path << std::endl;

        //fpga_init(base_address, interrupt_path);
        //_uio.fpga_init(base_address, interrupt_path);

        // 设置cmd rtcode
        unsigned char cmd = CMD::BEGIN;
        uint8_t rtcode = ERR_STATUS::NONE;//没有错误的话输出

        return simple_cmd_reply(lhs, cmd, rtcode);
    }
}

Message && WorkTask::cmd_tx_data_reply(const Message &lhs)
{
    input_info_t input_info;
    std::vector<char> payload = lhs.get_real_payload();

    memcpy(&input_info, payload.data(), sizeof(input_info));

    if(!input_data_check(input_info)){ // error return 
        cout << ">> receiver image error !" << endl;
        
        //set cmd rtcode
        unsigned char cmd = CMD::TX_DATA;
        uint8_t rtcode = ERR_STATUS::PARAM;
        
        return simple_cmd_reply(lhs, cmd, rtcode);
    }else{
        DATA_READ_ADDRESS_OFFSET = input_info.input_address;
        work_info.input_info = input_info;
        cout << "DATA_READ_ADDRESS_OFFSET : " << DATA_READ_ADDRESS_OFFSET << endl;
        
        _ps.ps_copy_inputdata(payload.data() + sizeof(input_info),
                              payload.size() - sizeof(input_info), DATA_READ_ADDRESS_OFFSET);
        
        // set cmd trcode
        unsigned char cmd = CMD::TX_DATA;
        uint8_t rtcode = ERR_STATUS::NONE;

        return simple_cmd_reply(lhs, cmd, rtcode);
    }
}

Message && WorkTask::cmd_calc_reply(const Message &lhs)
{
    int reg_index = -1;

    if(_ps.ps_idle_check()/*ps_idle_check()*/){

        cout << "cmd test reg" << endl;

        reg_index = /* cmd_test_reg() */ 0;
        
        // for cmd  rtcode
        unsigned char cmd = CMD::START_CAL;
        uint8_t rtcode = reg_index;

        return simple_cmd_reply(lhs, cmd, rtcode );
    }else {
        
        // for cmd rtcode
        unsigned char cmd = CMD::START_CAL;
        uint8_t rtcode = ERR_STATUS::BUSY;

        return simple_cmd_reply(lhs, cmd, rtcode);
    }
}

Message && WorkTask::cmd_get_data_reply(const Message &lhs)
{    
}

Message && WorkTask::cmd_status_reply(const Message &lhs)
{
    unsigned int status = /* ps_status() */ 0; //**
    vector<char> databuf;
    databuf.reserve(sizeof(unsigned int));

    memcpy(databuf.data(), &status, sizeof(unsigned int));

    // for cmd rtcode
    unsigned char cmd = CMD::PL_STATUS;
    uint8_t rtcode = ERR_STATUS::NONE;

    return simple_cmd_reply_data(lhs, cmd, rtcode, databuf);
}

Message && WorkTask::cmd_config_reply(const Message &lhs)
{
    FILE *fp =  fopen("/run/myconfig.txt","w");
    if(fp == nullptr){
        
        // for cmd  rtcode
        unsigned char cmd = CMD::CONFIG;
        uint8_t rtcode = ERR_STATUS::NORM;

        return simple_cmd_reply(lhs, cmd, rtcode);
    }else{
        vector<char> payload = lhs.get_real_payload();
        fwrite(payload.data(), payload.size(), 1, fp);
        fclose(fp);
        
        // for cmd rtcode
        unsigned char cmd = CMD::CONFIG;
        uint8_t rtcode = ERR_STATUS::NONE;

        return simple_cmd_reply(lhs, cmd, rtcode);
    }
}

Message && WorkTask::cmd_test_reg_reply(const Message &lhs)
{
    vector<char> data = lhs.get_real_payload();
    size_t datalen  = data.size();

   // ps_copy_para(data);

    //ps_copy_para();
    
    // for cmd rtcode
    unsigned char cmd = CMD::TEST_REG;
    uint8_t rtcode = ERR_STATUS::NONE;

    return simple_cmd_reply(lhs, cmd, rtcode);
}

}// end of namespace snowlake
