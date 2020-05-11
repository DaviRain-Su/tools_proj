#pragma once
#include "Uio.h"

#include <iostream>
#include <vector>
using std::vector;

namespace  snowlake{

class PsProcess
{
public:
    PsProcess(const vector<int> &lhs);
    PsProcess(std::initializer_list<int> init);
    PsProcess();
    ~PsProcess() {}

    char* get_memBaseAddr() const;
    vector<int> &get_calc_data();
    int get_calc_len();
    bool ps_idle_check();
    int ps_clac_ok();
    int ps_status();
    int get_reg_value(int index);
    bool ps_copy_inputdata(void* pdata, unsigned int len, uint64_t data_address_offset);
    bool ps_copy_para(void *pdata, unsigned int len);
private:
    Uio _uio;
    static const int SIZE = 65535;
    vector<int> _buff;    
};


}
// end of namespace snowlake
