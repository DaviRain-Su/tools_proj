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
    int cmd_process(int socket, unsigned char* cmdbuf, unsigned int len);
    int cmd_test_reg_reply(int socket, unsigned char *cmdbuf, unsigned int len);

    int feed_dog();

private:
    static uint32_t OUT_PUT_ADDRESS; // 当前输入参数大小
    static uint32_t DATA_READ_ADDRESS_OFFSET; // 当前输出数据大小
    
    Frame_head m_frame_head;
    Work_info work_info;
    shared_ptr<Reg> ptr_reg;
    shared_ptr<PS_process> ptr_ps;
};

uint32_t Protocol::OUT_PUT_ADDRESS = 0;
uint32_t Protocol::DATA_READ_ADDRESS_OFFSET = 0;

Protocol::Protocol()
   // : ptr_reg(new Reg)
    : ptr_ps(new PS_process)
    {
        cout << "Protocol()"<<  endl;
    }

Protocol::~Protocol() {
    cout << "~Protocol()" << endl;
}

int
Protocol::input_data_check(Input_info_t &p_addr_size_info){
    return 1; 
}
int Protocol::init_data_check(Init_info_t& lhs){
    return 1;
}
int Protocol::output_data_check(Output_info_t &lhs){
    return 1;
}

int Protocol::simple_cmd_reply(int socket, uint16_t seq, unsigned char cmd, int8_t rtcode){

    unsigned char txbuf[FRAME_HEAD_LEN + BODY_INFO_LEN + END_DATA_LEN]; // 1 is FRAME_END_SYM

    F_Head *phead = (F_Head *)txbuf;
    phead->set_pkt_head(FRAME_HEAD_LEN + BODY_INFO_LEN, seq, 0);

    DBG_INFO("simple_cmd_reply cnd:%d\n\n", cmd);
    if(Frame_head::CMD::CMD_CONFIG == cmd){
        DBG_INFO("-----------END----------------\n\n");
    }

    txbuf[FRAME_HEAD_LEN] = cmd;
    txbuf[FRAME_HEAD_LEN + CMD_STATUS_LEN] = rtcode;

    unsigned int csum = check_sum(txbuf, 0xffffffff, FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN);

    memcpy(txbuf + FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN, &csum, CHECK_SUM_LEN);
    txbuf[FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + CHECK_SUM_LEN] = FRAME_END_SYM;

    int send_len;
    // fix for FRAME_END_SYM
    if((send_len = send(socket, txbuf, FRAME_HEAD_LEN + BODY_INFO_LEN + END_DATA_LEN, 0)) != (FRAME_HEAD_LEN + BODY_INFO_LEN + END_DATA_LEN)) {
        DBG_INFO("simple_cmd_reply send failed, socket recv %d\n", send_len);

        return -1;
    }
    return 0;
}

int simple_cmd_reply_data(int socket, uint16_t seq, unsigned char cmd, int8_t rtcode, void *databuf, unsigned int datalen){

    unsigned char txbuf[FRAME_HEAD_LEN + BODY_INFO_LEN + MYCONFIG_MAX_LEN + END_DATA_LEN]; // 1 is for FRAME_END_SYM
    if(MYCONFIG_MAX_LEN < datalen){
        DBG_INFO("simple_cmd_reply_data is too large.");
        return -1;
    }

    F_Head *phead = (F_Head*)txbuf;
    phead->set_pkt_head(FRAME_HEAD_LEN + BODY_INFO_LEN + datalen, seq, 0);
    DBG_INFO("simple_cmd_reply_data : %d\n\n", cmd);

    txbuf[FRAME_HEAD_LEN] = cmd;
    txbuf[FRAME_HEAD_LEN + CMD_STATUS_LEN] = rtcode;

    memcpy(txbuf + FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN, databuf, datalen);
    unsigned int csum = check_sum(txbuf, 0xffffffff, FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + datalen);
    memcpy(txbuf + FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + datalen, &csum, CHECK_SUM_LEN);
    //fix for FRAME_END_SYM
    //txbuf[FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + datalen + CHECK_SUM_LEN] = FRAME_END_SYM;

    int send_len;
    // fix for FRAME_END_SYM
    //if( ( send_len = send(socket, txbuf, FRAME_HEAD_LEN + BODY_INFO_LEN + datalen + END_DATA_LEN , 0)) != (FRAME_HEAD_LEN + BODY_INFO_LEN + datalen + END_DATA_LEN)) {
    if( ( send_len = send(socket, txbuf, FRAME_HEAD_LEN + BODY_INFO_LEN + datalen , 0)) != (FRAME_HEAD_LEN + BODY_INFO_LEN + datalen)) {
        DBG_INFO("simple_cmd_reply_data send failed, socket recv %d\n", send_len);
        return -1;
    }
    return 0;
}

// need
int
Protocol::cmd_begin_reply(int socket, unsigned char * cmdbuf, unsigned int len){
    //----- 
    Init_info_t init_info;
    F_Head *phead = (F_Head*)cmdbuf;
    memcpy(&init_info, cmdbuf + FRAME_HEAD_LEN + 1, sizeof(Input_info_t));

    if( !init_data_check(init_info))
    {
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD_BEGIN, Frame_head::ERR_STATUS::ERR_PARAM);
    }else{
        // -----
        unsigned int base_address = (unsigned int )(init_info.base_address & 0xffffffff);
        // -----
        char* interrupt_path = init_info.interrupt_path;
        // -----
        //ptr_reg->fpga_init(base_address, interrupt_path);
        ptr_reg = std::make_shared<Reg>(new Reg);

        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_BEGIN, Frame_head::ERR_STATUS::ERR_NONE);
    }
}

// need
int Protocol::cmd_tx_data_reply(int socket, unsigned char* cmdbuf, unsigned int len){
    // -----
    Input_info_t input_info;
    F_Head * phead = (F_Head*)cmdbuf;
    memcpy(&input_info, cmdbuf + FRAME_HEAD_LEN + 1, sizeof(Input_info_t));
    // input_data_check() ----
    if(input_data_check(input_info)){ // input_data_check 有问题
        // -----
        DATA_READ_ADDRESS_OFFSET  = input_info.input_address;
        // ------
        //work_info.input_info = input_info;
        work_info.set_init_info(input_info);
        DBG_INFO("DATA_READ_ADDRESS_OFFSET : 0x%llx\n", DATA_READ_ADDRESS_OFFSET);
    }else{
        DBG_INFO("receiver image error! \n");
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD_TX_DATA, Frame_head::ERR_STATUS::ERR_PARAM);
    }
    // ----
    ptr_ps->ps_copy_input_data(work_info.input_info, cmdbuf + FRAME_HEAD_LEN + 1 + sizeof(Input_info_t), phead->get_len() - FRAME_HEAD_LEN - sizeof(input_info) - CMD_STATUS_LEN - CHECK_SUM_LEN - CHECK_SUM_LEN - END_DATA_LEN, DATA_READ_ADDRESS_OFFSET);

    return simple_cmd_reply(socket, phead->get_len(), Frame_head::CMD::CMD_TX_DATA, Frame_head::ERR_STATUS::ERR_NONE);
}

// need
int Protocol::cmd_calc_reply(int socket, unsigned char *cmdbuf, unsigned int len){

    F_Head * phead = (F_Head*)cmdbuf;
    int reg_index = -1;
    // ----
    if(ptr_ps->ps_idle_check()) {
        DBG_INFO("cmd_test_red\n");
        // ------
        reg_index = ptr_reg->cmd_test_reg();
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_START_CAL, reg_index);
    }else{
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_START_CAL, Frame_head::ERR_STATUS::ERR_BUSY);
    }
}

int Protocol::cmd_config_reply(int socket, unsigned char *cmdbuf, unsigned int len){

    F_Head *phead = (F_Head*)cmdbuf;
    FILE *fp = fopen("/run/myconfig.txt", "w");

    if(fp == NULL){
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_CONFIG, Frame_head::ERR_STATUS::ERR_NORM);
    }
    // -----
    fwrite(cmdbuf + FRAME_HEAD_LEN + 1, phead->get_len() - FRAME_HEAD_LEN - 6, 1, fp);// -6 ? 
    fclose(fp);

    return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_CONFIG, Frame_head::ERR_STATUS::ERR_NONE);
}

int Protocol::cmd_get_data_reply(int socket, unsigned char* cmdbuf, unsigned int len){

    Output_info_t out_info;
    F_Head *phead = (F_Head*)cmdbuf;
    // -----
    memcpy(&out_info, cmdbuf + FRAME_HEAD_LEN + 1, sizeof(out_info));

    uint32_t out_put_address_offset = 0;
    uint32_t out_put_size = 0;
    if(output_data_check(out_info)){
        out_put_address_offset = out_info.output_address;
        out_put_size = out_info.output_data_size;
        DBG_INFO("OUT_PUT_ADDRESS : 0x%llx, OUTPUT_PARA_SIZE: 0x%llx\n", out_put_address_offset, out_put_size);
    }

    int data_len = 0;
    unsigned char txbuf[100];

    // ----
    if(ptr_ps->ps_calc_ok()){// 计算ok
        data_len = out_put_size;// 图片真实长度
        DBG_INFO("get_calc_len : 0x%llx\n", data_len);

        // send : HEAD + CMD_GET_DATA + ERR_NONE
        F_Head *ptxbufhead = (F_Head*)txbuf;
        ptxbufhead->set_pkt_head(FRAME_HEAD_LEN + STATUS_INFO_LEN + ERR_STATUS_LEN + CMD_STATUS_LEN + data_len + CHECK_SUM_LEN, phead->get_seq(), 0);
        txbuf[FRAME_HEAD_LEN] = Frame_head::CMD::CMD_GET_DATA;
        txbuf[FRAME_HEAD_LEN + 1] = Frame_head::ERR_STATUS::ERR_NONE;

        memcpy(txbuf + (FRAME_HEAD_LEN + ERR_STATUS_LEN + CMD_STATUS_LEN), &data_len, STATUS_INFO_LEN);
        // fix add FRAME_END_SYM
        //txbuf[FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN] = FRAME_END_SYM;

        int send_len = 0;
        // fix add FRAME_END_SYM
        //if( (send_len = send(socket, txbuf, FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN + END_DATA_LEN, 0)) != (FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN + END_DATA_LEN)) {
        if( (send_len = send(socket, txbuf, FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN, 0)) != (FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN )) {
            DBG_INFO("send head failed, send_len = %d\n", send_len);
            return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_GET_DATA, Frame_head::ERR_STATUS::ERR_NORM);
        }

        unsigned int csum = check_sum(txbuf, 0xffffffff, FRAME_HEAD_LEN + CMD_STATUS_LEN + ERR_STATUS_LEN + STATUS_INFO_LEN);

        unsigned char * recv_data = nullptr;

        // 发送计算后图片：　首地址　memBaseAddr + offset
        recv_data = (unsigned char*)malloc(sizeof(unsigned char) * (data_len + 1));

        memset(recv_data, 0, data_len + 1);
        memcpy(recv_data, ptr_reg->get_memBasseAddr() + out_put_address_offset, data_len);
        DBG_INFO("send data offset: %d, data_len : %d\n", out_put_address_offset, data_len);
        
        int temp = send(socket, recv_data, data_len, 0);
        DBG_INFO("send data offset: %d, data_len : %d, send_temp %d\n", out_put_address_offset, data_len, temp);
        if( (send_len = temp) != data_len ){
            DBG_INFO("send_photo failed , send_len = %d\n", send_len);
            free(recv_data);
            return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_GET_DATA, Frame_head::ERR_STATUS::ERR_NORM);
        }
        DBG_INFO("send photo success, send_len = %d, memBaseAddr = %x\n", send_len, ptr_reg->get_memBasseAddr());
        free(recv_data);  

        //发送校验码+ 结束符 /0x0
        memset(txbuf, 0, sizeof(txbuf));
       
        memcpy(txbuf, &csum, CHECK_SUM_LEN);
        txbuf[CHECK_SUM_LEN] = FRAME_END_SYM;

        if((send_len = send(socket, txbuf, CHECK_SUM_LEN + END_DATA_LEN, 0)) != (CHECK_SUM_LEN + END_DATA_LEN)){
            DBG_INFO("send scum + end failed, send_len = %d\n", send_len);
            return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_GET_DATA, Frame_head::ERR_STATUS::ERR_NORM);
        }

        return 0; // 
    }else{//计算错误或未完成
        return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_GET_DATA, Frame_head::ERR_STATUS::ERR_NORM);
    }
}

int Protocol::cmd_test_reg_reply(int socket, unsigned char* cmdbuf, unsigned int len) {
    F_Head* phead = (F_Head*)cmdbuf;
    // ps_copy_para
    ptr_ps->ps_copy_para(cmdbuf + FRAME_HEAD_LEN + 1, phead->get_len() - FRAME_HEAD_LEN - CMD_STATUS_LEN - CHECK_SUM_LEN - END_DATA_LEN);

    return simple_cmd_reply(socket, phead->get_seq(), Frame_head::CMD::CMD_TEST_ERG, Frame_head::ERR_STATUS::ERR_NONE);
}

int Protocol::cmd_status_reply(int socket, unsigned char* cmdbuf, unsigned int len){
    F_Head* phead = (F_Head*)cmdbuf;
    // ---------
    unsigned int status = ptr_ps->ps_status();

    return simple_cmd_reply_data(socket, phead->get_seq(), Frame_head::CMD::CMD_PL_STATUS, Frame_head::ERR_STATUS::ERR_NONE, &status, STATUS_INFO_LEN);
}

int Protocol::cmd_process(int socket, unsigned char* cmdbuf, unsigned int len) {
    unsigned char cmd = cmdbuf[FRAME_HEAD_LEN];

    int index = 0;
    switch (cmd) {
    case Frame_head::CMD::CMD_BEGIN:
        DBG_INFO("CMD_START\n");

        return cmd_begin_reply(socket, cmdbuf, len);
    case Frame_head::CMD::CMD_TX_DATA:
        DBG_INFO("TX_DATA\n");

        return cmd_tx_data_reply(socket, cmdbuf, len);
    case Frame_head::CMD::CMD_START_CAL:
        DBG_INFO("CALC\n");

        return cmd_calc_reply(socket, cmdbuf, len);
    case Frame_head::CMD::CMD_GET_DATA:
        DBG_INFO("GET_DATA\n");

        index = cmd_get_data_reply(socket, cmdbuf, len);

        return index;
    case Frame_head::CMD::CMD_PL_STATUS:
        DBG_INFO("CMD_PL_STATUS\n");

        return cmd_status_reply(socket, cmdbuf, len);
    case Frame_head::CMD::CMD_CONFIG:
        DBG_INFO("GET_CONFIG\n");

        return cmd_config_reply(socket, cmdbuf, len);
    case Frame_head::CMD::CMD_TEST_ERG:
        DBG_INFO("CMD_PARA\n");

        return cmd_test_reg_reply(socket, cmdbuf, len);
    default:
        DBG_INFO("unkown cmd\n");

        return 0;
    }
}

};// end of namespace snowlake;
