#pragma once

#include <iostream>
#include <memory>


#include "Reg.h"
#include "utils.h"
#include "info_struct.h"


using std::shared_ptr;

namespace  snowlake { // start namespace snowlake 

class PS_process
{

public:
    PS_process();
    ~PS_process();

    void *get_calc_data();
    int get_calc_len();
    int ps_idle_check();
    int get_reg_value(int index);
    int ps_calc_ok();
    int ps_status();
    int ps_copy_input_data(Input_info_t &paddr_size_info, void * pdata, unsigned int len, uint64_t data_address_offset);
    int ps_copy_para(void *pdata, unsigned int len);

private:
    shared_ptr<Reg> ptr_reg;
};
};// end of namespace std
