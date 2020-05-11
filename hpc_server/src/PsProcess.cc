#include "../inc/PsProcess.h"

namespace  snowlake{

PsProcess::PsProcess(const vector<int> &lhs)
    :_uio()
{
        _buff.insert(_buff.begin(), lhs.begin(), lhs.end());
}

PsProcess::PsProcess(std::initializer_list<int> init)
    :_uio()
{
    _buff.insert(_buff.begin(), init.begin(), init.end());
}

PsProcess::PsProcess()
    :_uio()
{
        _buff.reserve(SIZE);
        for(int i = 0; i < SIZE; i++){
            _buff[i] = i;
        }
}


char* PsProcess::get_memBaseAddr() const 
{
    return _uio.get_memBaseAddr();
}

vector<int>&  PsProcess::get_calc_data()
{
    return _buff;
}

int PsProcess::get_calc_len() 
{
    return 512 * 512 * 3;
}

bool PsProcess::ps_idle_check() 
{
    return true;
}

int PsProcess::get_reg_value(int index)
{
    int32_t real_value = 0;

    int length = _uio.get_reg_len();
    if(0 < length){
        if(length - 1 < index){
            std::cout << "Index more than regs length " << std::endl;
            return -1;
        }else {
            real_value = _uio.get_regs_index_value(index);
        }
    }else{
        std::cout << "Regs length is empty" << std::endl;
    }

    return real_value;
}

int PsProcess::ps_clac_ok()
{
    int length = _uio.get_reg_len();

    if(0 < length){
        unsigned int *regs = nullptr;
        regs = (unsigned int*)malloc(sizeof(unsigned int*)*length);

        int len = _uio.get_regs(regs, length);
        if(0 == len){
            std::cout << "ps_status parse  regs fail " << std::endl;
            free(regs);
            return -1;//ERR_PARAM
        }
        free(regs);
    }
    return 0;
}

int PsProcess::ps_status()
{
    int length = _uio.get_reg_len();

    if(0 < length){
        unsigned int *regs = nullptr;
        regs = (unsigned int*)malloc(sizeof(unsigned int*)*length);

        int len = _uio.get_regs(regs, length);
        if(0 == len){
            std::cout << "ps_status parse  regs fail " << std::endl;
            free(regs);
            return -1;//ERR_PARAM
        }

        if(1 == regs[0]){
            std::cout << "regs[0] = " << regs[0] 
                << "AXI works well " << std::endl;
            free(regs);
            return 0;// ERR_AXI_SUC
        }else{
            std::cout << "AXI works works failed, regs[0] = " << regs[0] 
                << std::endl;
            free(regs);
            return -1;//ERR_PARAM
        }
    }

    return -1;//ERR_PARAM;
}

bool PsProcess::ps_copy_inputdata(void* pdata, unsigned int len, uint64_t data_address_offset)
{
    uint64_t offset = data_address_offset;
    uint64_t memBasrCopyAddr = (uint64_t )get_memBaseAddr() + offset;

    std::cout << ">> ps_copy_inpudata " 
        << "data_address_offset " << offset
        << "memBaseAddr " << get_memBaseAddr()
        << "memBasrCopyAddr " << memBasrCopyAddr << std::endl;

    memcpy((void*)memBasrCopyAddr, pdata, len);

    return true;
}

bool PsProcess::ps_copy_para(void *pdata, unsigned int len)
{
    uint64_t offset = 0x0;
    std::cout << " >> ps_copy_para len =  " << len << ", memBaseAddr = " << get_memBaseAddr() << std::endl;
    char *temp_addr = get_memBaseAddr() + offset;
    memcpy(temp_addr, pdata, len);

    return true;
}

}
// end of namespace snowlake
