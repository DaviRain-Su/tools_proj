#pragma once

#ifndef __SNOWLAKE_BUFFER_H_
#define __SNOWLAKE_BUFFER_H_

#include "Message.h"

#include <string.h>

#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;
using std::endl;
using std::cout;

namespace  snowlake {

enum CMD {
   BEGIN = 1,
   TX_DATA,
   START_CAL,
   GET_DATA,
   PL_STATUS,
   CONFIG,
   TEST_REG,
};

enum ERR_STATUS{
    NONE = 0,
    NORM,
    PARAM,
    BUSY,
    PARA,
    AXI_SUC,
};

// fream head
typedef struct pkt_head{
    uint32_t preamble;
    uint32_t len;
    uint32_t ver;
    uint32_t total_package_len;
    uint16_t seq;
    uint16_t flags;
}Pkt_head;

class Buffer
{
public:
    Buffer();
    
    Buffer(int size);

    string &&  encode(const Message &lhs);

    Message && decode(const string &lhs);

    vector<char>& getBuff();

    int get_realSize() const;
    
    bool fream_head_is_vaild(Pkt_head *pkt_head);
    
    ~Buffer(){}

private:
    static const uint32_t Frame_preamble = 0xBE00A55A;
    
    // 500M的大小
    static const int SIZE = 500 * (1 << 20);//发送的数据包的最大是1045字节
    
    static const int Frame_min_len = sizeof(struct pkt_head)  + 2 + 4 + 1 ;//最小包的长度
    
    static const int Frame_max_len = sizeof(struct pkt_head) + SIZE + 4 + 1;
    
    vector<char> buff_;// 实际上要发送的char数据

    int _realSize;//实际写入的数字
    //Message _message;//用于解析后存储, 并且返回解析后的数据
    //在encode的时候用于需要发送的数据的存储
};

}// end of namespace snowlake
#endif
