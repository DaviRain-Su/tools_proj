#pragma once
#include <string.h>// memcpy

#include <vector>
#include <iostream>
#include <string>
using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace  snowlake {

class Message
{
public:
    Message();

    Message(uint32_t len, uint32_t seq, const string &payload, uint32_t checksum);

// ----------set info ----------------------
//
    void set_preamble(uint32_t preamble);

    void set_len(uint32_t len);

    void set_version(uint32_t ver);

    void set_total_package_len(uint32_t len);

    void set_seq(uint16_t seq);

    void set_flag(uint16_t flag);
    
    //for string
    void set_payload(const string & lhs);
    
    //for char
    void set_payload(const char& lhs);
    
    void set_cmd(uint8_t cmd);

    void set_real_payload(const string &lhs);

    void set_checksum(const string& checksum);

    void set_end_sym(const uint8_t sym);

// -------- get info ------------------
//
    uint32_t get_preamble() const;

    uint32_t get_len() const;

    uint32_t get_version() const;

    uint32_t get_total_package_len() const;

    uint16_t get_seq() const;

    uint16_t get_flag() const;

    vector<char>& get_payload();

    vector<char> get_payload() const;
    
    uint8_t get_cmd() const;

    vector<char> && get_real_payload() const;

    vector<char> get_checksum() const;

    uint8_t get_end_sym() const;


    //用于求md5码
    string && get_Message_string(); 

// ----------get info size ----------------
//
    int get_preamble_size() const; 

    int get_len_size() const;

    int get_version_size() const;

    int get_total_package_len_size() const;

    int get_seq_size() const;

    int get_flag_size() const;

    int get_cmd_status_size() const;

    int get_err_status_size() const;

    int get_end_sym_size() const;

    int get_myconfig_size() const;

    int get_checksum_size() const;

    void to_string();
    
    int get_full_package_size() const;
    ~Message() {}
private:
    int get_Message_size() const;

private:
    // Frame_preamble 
    static const uint32_t Frame_preamble = 0xBE00A55A; // 固定的Frame preamble

    // Frame_end_sym
    static const uint16_t Frame_end_sym = 0x99; // 固定的Frame end sym 

    //Frame_version 
    static const uint32_t Frame_version = 1;//固定的Frame version

    //Frame_flag
    static const uint16_t Frame_flag = 0;//固定的Frame flag


    static const int Err_status_len = 1;//应答错误码的长度

    static const int Cmd_status_len = 1;//交互状态应答码的长度

    static const int End_sym_len = 1;//结束符的长度

    // check sum size  32
    static const int Frame_checksum_size = 32;//完整性校验码长度

    static const int Myconfig_max_len = 1024;

    //preamble_
    uint32_t preamble_;// 前导符 4字节, 用于区分这个数据是不是message报文

    // len_
    uint32_t len_;//包长度 4字节 --> 发送一个数据需要发送的报文的总的个数大小

    // version_
    uint32_t version_;//版本号 4字节 --> 设计软件的版本号

    //total_package_len

    uint32_t total_package_len_; // 拆解后包的个数大小

    // seq_
    uint16_t seq_; //包序号 2字节 ---> 发送报文包的序号，这是一个数据差分为多个数据去发送

    // flag_
    uint16_t flag_;//标志位 2字节 ---> 保留标志位

    // payload_
    vector<char> payload_;//数据内容,数据包带外数据()
    // 不含需要发送额外的数据话，payload的最小的报文长度时2;
    // 包含的是：应答错误码一个字节， 交互状态应答状态码长度
    // 因此最小的payload是2字节
    // 最大的字节是1024字节

    // checksum 
    vector<char> checksum_;//完整性校验 4字节

    // end sym
    uint8_t end_sym_;//结束符 1字节
    // 因此一个报文的最小报文是 4 + 4 + 4 + 2 + 2 + 2 (min) + 4 + 1 = 23字节
    // 最大的一个报文是4 + 4 + 4 + 2 + 2 + 1024(max) + 4 + 1 = 1045字节
};

}// end of namespace snowlake

