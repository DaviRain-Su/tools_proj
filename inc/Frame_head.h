#pragma once

#include <iostream>
#include "utils.h"

namespace  snowlake { // namespace snowlake

const size_t FRAME_HEAD_LEN = sizeof(pthread_key_t) ;//包长度目前16
const size_t FRAME_MIN_LEN = FRAME_HEAD_LEN + 1 + 1; // 最小包长度
const size_t FRAME_MAX_LEN = FRAME_HEAD_LEN + 20 * (1 << 20);//最大包长度
const uint32_t FRAME_PREAMBLE = 0xBE00A55A; // 前导符,　在这个类用作判断使用
const uint8_t FRAME_END_SYM = 0x09;//结束符
const size_t MYCONFIG_MAX_LEN = 1024; // Myconfig.txt 的最大长度设置

/*数据包结构长度定义，　完整数据包应包含: FRAME_HEAN_LEN + BODY+INFO_LEN 数据长度*/

const uint8_t BODY_INFO_LEN = 7;//ERR_STATUS_LEN + CMD_STATUS_LEN + CHECK_SUM_LEN + END_DATA_LEN
const uint8_t CMD_STATUS_LEN = 1;// 交互状态应答状态码长度
const uint8_t ERR_STATUS_LEN = 1;// 应答错误码长度
const uint8_t CHECK_SUM_LEN = 4;//完整行校验码长度
const uint8_t END_DATA_LEN = 1;//结束符长度

const uint8_t STATUS_INFO_LEN = 4;//STATUS 长度

class pkt_head_t{
    uint32_t m_preamble;//前导符
    uint32_t m_len;//包长度
    uint32_t m_ver;//版本号
    uint16_t m_seq;//包序号
    uint16_t m_flag;//标志位
public:
    pkt_head_t();
    ~pkt_head_t(){}
    void set_pkt_head(uint32_t len, uint16_t seq, uint16_t flags);
    uint32_t get_preamble() const ;
    uint32_t get_len() const ;
    uint16_t get_seq() const ;
    uint16_t get_flag() const ;
    unsigned int get_pkt_data_len() const ;
    void set_preamble(uint32_t lhs) ;
};



typedef pkt_head_t F_Head;





class Frame_head
{
public:
    /*应答错误码　ERR_STATUS*/
    enum ERR_STATUS {
        ERR_NONE = 0,   //无错
        ERR_NORM = 1,   //一般性错误
        ERR_PARAM = 2,  //参数错误
        ERR_BUSY = 3,   //设备忙
        ERR_PARA = 4,   //参数未成功初始化
        ERR_AXI_SUC = 5,//AXI总线工作正常
    };
    /*包接受状态*/
    enum RX_STATE {
        RX_STATE_START = 0,
        RX_STATE_HEAD_GOT = 1,
    };
    /*包命令定义*/
    enum CMD {
        CMD_BEGIN = 1,
        CMD_TX_DATA = 2,
        CMD_START_CAL = 3,
        CMD_GET_DATA = 4,
        CMD_PL_STATUS = 5,
        CMD_CONFIG = 6, 
        CMD_TEST_ERG = 7,
    };

public:
    Frame_head() ;
    ~Frame_head() ;

    void set_frame_head(F_Head* phead);
private:
    //包头长度，目前16字节
    size_t get_frame_head_len() const;
    // 最小包长度
    size_t get_frame_min_len() const;
    //最大包长度
    size_t get_frame_max_len() const;
    size_t get_body_info_len() const;
    size_t get_err_status_len() const;
    size_t get_cmd_status_len() const;
    size_t get_end_data_len() const;
    size_t get_check_sum_len() const;
    size_t get_status_info_len() const;
    uint32_t get_frame_head_preamble() const;
public:
    bool freamehead_is_vaild();
    
    // 包长度
    unsigned int get_frame_total_len();
    // 数据部分长度
    unsigned int get_frame_data_len();
    void frame_fill_head(uint32_t len, uint16_t seq, uint16_t flags);
private:
    F_Head *m_f_head;
};




}; // end of namespace snowlake
