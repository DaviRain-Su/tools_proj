#pragma once

#include <iostream>

#include "uio.h"

class Ps_process
{
    //初始化信息
    struct Init_info_t {
        uint64_t base_address;//基地址
        char interrupt_path[12];//中断文件路径

        Init_info_t() {}
        ~Init_info_t() {}
    };
    // input offset info
    struct Input_info_t {
        uint64_t input_address;// 读入地址  
        
        Input_info_t() {}
        ~Input_info_t() {}
    };
    struct Output_info_t {
        uint64_t output_address;//输出地址
        uint64_t output_data_size;//data size
        
        Output_info_t() {}
        ~Output_info_t() {}
    };
    struct Reg_index_t {
        uint64_t index_value;// index
        
        Reg_index_t() {}
        ~Reg_index_t() {}
    };

public:
    Ps_process() {}
    ~Ps_process() {}
    
    void *get_calc_data(void);
    int get_calc_len(void);
    int ps_idle_check(void);
    int ps_status(void);
    int get_reg_value(int index);
    int ps_copy_input_data(Init_info_t &paddr_size_info, void * pdata, unsigned int len, uint64_t data_address_offset);
    int ps_copy_para(void *pdata, unsigned int len);
private:
    Init_info_t init_info;
    Input_info_t input_info;
    Output_info_t output_info;
    Reg_index_t reg_index;
};

