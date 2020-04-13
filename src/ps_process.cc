#include "../inc/ps_process.h"

namespace  snowlake{ //start  namespace snowlake

PS_process::PS_process()
    : ptr_reg (new Reg)
{
    std::cout << "PS_process()" << std::endl;
}

PS_process::~PS_process() {
    std::cout << "~PS_process()" << std::endl;
}

void* PS_process::get_calc_data(){
    static unsigned char buff[1024 * 1024] = {2, 3, 4, 5, 6, 7, 8, 9 , 10};
    return buff;
}

int PS_process::get_calc_len(){
    return 512 * 512 * 3;
}
int PS_process::ps_idle_check() {
    return true;
}

int PS_process::get_reg_value(int index){
    int32_t real_value = 0; 

    int length = ptr_reg->get_regs_len();
    if( 0 < length ){
        if (length - 1 < index ){
            printf("index more than regs length\n");
            return -1;
        }
        real_value = ptr_reg->get_regs_index_value(index);
    }else{
        printf("reg length is empty!\n");
    }
    return real_value;
}

int PS_process::ps_calc_ok(){
    int length = ptr_reg->get_regs_len();
    printf("ps_calc_ok length = %d\n", length);

    if(0 < length ){
        unsigned int *regs = nullptr;
        regs = (unsigned int*)malloc( sizeof(unsigned int) * length );

        int len = ptr_reg->get_regs(regs, length);
        if(0 == len ){
            printf("parse regs fail\n");
            free(regs);
            return -1;
        }
        free(regs);
    }
    return 0;
}

int PS_process::ps_status(){
    int length = ptr_reg->get_regs_len();

    if(0 < length){ 
        unsigned int *regs = nullptr;
        regs = (unsigned int*)malloc(sizeof(unsigned int) * length);

        int len = ptr_reg->get_regs(regs, length);
        if(0 == len){
            printf("ps_status parse regs fail\n");
            free(regs);
            return Frame_head::ERR_STATUS::ERR_PARAM;
        }

        if(1 == regs[0]){
            printf("regs[0] == 0x%x, AXI works well \n", regs[0]);
            free(regs);
            return Frame_head::ERR_STATUS::ERR_AXI_SUC;
        }else{
            printf("AXI works failed, regs[0] = 0x%x\n", regs[0]);
            free(regs);
            return Frame_head::ERR_STATUS::ERR_PARAM;
        }
    }else{
        return Frame_head::ERR_STATUS::ERR_PARAM;
    }
}

int PS_process::ps_copy_input_data(Input_info_t &paddr_size_info, void *pdata, unsigned int len, uint64_t data_address_offset)
{
    uint64_t offset = data_address_offset;
    void *memBaseCopyAddr = ptr_reg->get_memBasseAddr() + offset;

    DBG_INFO("[T]ps_copy_inputdata || data_address_offset : 0x%llx, memBaseAddr: 0x%llx, memBaseCopyAddr: 0x%llx, len: 0x%llx\n", offset, ptr_reg->get_memBasseAddr(), memBaseCopyAddr, len);
    memcpy(memBaseCopyAddr, pdata, len);

    return 0;

}

int PS_process::ps_copy_para(void *pdata, unsigned int len){
    uint64_t offset = 0x0;
    printf("[T]ps_copy_para %d, memBaseAddr = %x\n", len, ptr_reg->get_memBasseAddr());
    memcpy(ptr_reg->get_memBasseAddr() + offset,pdata, len);
    return 0;
}

};
