#pragma once
/*===========================================
 *
 * 文件名：
 *
 * 文件描述：
 *
 * 创建人：
 *
 * 版本：
 *
 * 修改记录：
 *
 ============================================ */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>


#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <fstream>

using std::ofstream;
using std::string;

#ifndef  __SNOWLAKE_UIO_H__
#define __SNOWLAKE_UIO_H__

namespace  snowlake {

//
// for Err_reply_cmd
//
enum ERR_REPLY_CMD {
    ERROR_CMD = 0,
    READ_CMD,
    WRITE_CMD,
    READ_ALL_CMD,
    WRTE_IMAGE_CMD,//可能以后会发生改变
    READ_IMAGE_CMD,
    TEST_CMD, // test_cmd
    QUIT_CMD,//quit cmd 
};

//
//mem_block_addr
//
#if 0
enum MEM_BLOCK_ADDR {
    MEM_BLOCK_BASE 	  = 0x40000000, //?
    MEM_BLOCK_1_OFFSET    = 0x00000000,//?
    MEM_BLOCK_2_OFFSET    = 0x00300000,//?
    MEM_BLOCK_3_OFFSET    = 0x00600000,//?
    MEM_BLOCK_4_OFFSET    = 0x00900000,//?
} mem_block_addr_t __attribute__((unused));
#else
enum MEM_BLOCK_ADDR {
    MEM_BLOCK_BASE 	  = 0x40000000, //?
    MEM_BLOCK_1_OFFSET    = 0x00000000,//?
    MEM_BLOCK_2_OFFSET    = 0x00300000,//?
    MEM_BLOCK_3_OFFSET    = 0x00600000,//?
    MEM_BLOCK_4_OFFSET    = 0x00900000,//?
};
#endif

//
//reg_index_e
//
static enum reg_index_e {
    REG_INDEX0	 = 0,
    REG_INDEX1,//24 bit including flag+Data Length
    REG_INDEX2,
    REG_INDEX3,
    REG_INDEX4,
    REG_INDEX5=5,
    REG_INDEX6=6,
    REG_INDEX7,
    REG_INDEX8,
    REG_INDEX9,
    REG_INDEX10,
    REG_INDEX11,
    REG_INDEX12,
    REG_INDEX13,
    REG_INDEX14,
    REG_INDEX15,
    REG_INDEX16,
    REG_INDEX17,
    REG_INDEX18,
    REG_INDEX19,
    REG_INDEX20,
    REG_INDEX21,
    REG_INDEX22,
    REG_INDEX23,
    REG_INDEX24,
    REG_INDEX25,
    REG_INDEX26,
    REG_INDEX27,
    REG_INDEX28,
    REG_INDEX29,
    REG_INDEX30,
} reg_index_t __attribute__((unused));

//
//for UIO
//
class Uio
{
public:
    Uio()
        :_fd(0),
        _fd0(0),
        _mfd(0),
        _memBaseAddr(nullptr),
        _mapAddr(nullptr),
        _mapAddrCt(nullptr),
        _map_addr(nullptr),
        _map_addrCt(nullptr),
        _mapped(nullptr)
    {}

    ~Uio() {}

    void copy_mem_to_file(const string &outfilename, int offset, int total_len);

    void copy_file_to_mem(const string &infilename, int offset);

    int get_regs(unsigned int *regs, int len);

    int get_regs();

    int get_reg_len();

    int get_regs_index_value(unsigned int reg_idex);

    int fpga_init(uint64_t mem_block_base, const string &interrupt_path);

    int fpga_exit();

    int cmd_test_reg();

    char* get_memBaseAddr() const 
    {
        return _memBaseAddr;
    }
private:
    static const int WRITE_SIZE = 8 * (1 << 20);

    static const int BUFFER_SIZE = 1 << 29;

    static const int SIZE = 512;

    static const int IMAGE_SIZE = SIZE * SIZE * 3;

    static const int MAP_SIZE = 4095;


    int _fd, _fd0, _mfd;

    char* _memBaseAddr;

    void* _mapAddr;

    void* _mapAddrCt;

    void* _map_addr;

    void* _map_addrCt;

    unsigned int* _mapped;
};

}
// end of namespace snowlake
#endif
