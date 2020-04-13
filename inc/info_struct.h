#pragma once

#include <iostream>

namespace  snowlake { // start snowlake 

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
    
    /*
    Init_info_t * operator=(Input_info_t lhs){
        input_address = lhs.input_address;
    }
    */

    void set_value(Input_info_t lhs){
        input_address = lhs.input_address;
    }

    Input_info_t() {
        std::cout << "input_address is " << input_address << std::endl;
    }
    ~Input_info_t() {
        std::cout << "~Input_info_t ()" << std::endl;
    }
};

// output offset and size info
struct Output_info_t {
    uint64_t output_address;//输出地址
    uint64_t output_data_size;//data size

    Output_info_t() {
        std::cout << "Output_info_t()" << std::endl; 
    }
    ~Output_info_t() {
        std::cout << "~Output_info_t()" << std::endl;
    }
};

// reg index
struct Reg_index_t {
    uint64_t index_value;// index

    Reg_index_t() {
        std::cout << "Reg_index_t()" << std::endl;
    }
    ~Reg_index_t() {
        std::cout << "~Reg_index_t()" << std::endl;
    }

};

}; // end of namespace std
