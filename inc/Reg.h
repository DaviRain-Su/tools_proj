#pragma once

#include <linux/watchdog.h> // watchdog
#include <sys/mman.h> // mmap
#include <stdio.h>// printf
#include <stdlib.h> // malloc
#include <unistd.h>
#include <string.h> // memcpy memset
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // open
#include <sys/ioctl.h> // ioctl

#include "utils.h"
#include "Frame_head.h"

namespace  snowlake { // namespace snowlake 

// read reg
#define ReadReg(BaseAddress, RegOffset) (*(volatile unsigned int *)((unsigned long)(BaseAddress) + RegOffset))
// write reg
#define WriteReg(BaseAddress, RegOffset, Data) *(volatile unsigned int *) ((unsigned long )(BaseAddress) + RegOffset) = Data

#define reg_print(fmt, ...) do { printf(GREEN"[%s]:" NONE fmt, __func__, ##__VA_ARGS__ ); }while(0)
#define reg_color_print(color, fmt, ...) do { printf(GREEN"[%s]:" color fmt NONE, __func__, ##__VA_ARGS__); }while(0)

class Reg
{
    //new Protocol
    enum mem_block_addr_e {
        MEM_BLOCK_BASE = 0x40000000, 
        MEM_BLICK_1_OFFSET = 0x00000000,
        MEM_BLOCK_2_OFFSET = 0x00300000,
        MEM_BLOCK_3_OFFSET = 0x00600000,
        MEM_BLOCK_4_OFFSET = 0x00900000,
    }mem_block_addr_t __attribute__((unused));
    
    enum HASH_CMD_e {
        ERROR_CMD, // error_cmd
        READ_CMD, // read_cmd
        WRITE_CMD, // write_cmd
        READ_ALL_CMD, // read_all_cmd
        WRITE_IMAGE_CMD, // write_image_cmd
        READ_IMAGE_CMD, // read_image_cmd
        TEST_CMD, // test_cmd
        QUIT_CMD, // quit_cmd
    };

    enum reg_index_e {
        REG_INDEX0 = 0,
        REG_INDEX1, // 24 bit including flag + Data Length
        REG_INDEX2,
        REG_INDEX3,
        REG_INDEX4,
        REG_INDEX5 = 5,
        REG_INDEX6 = 6,
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
public:
    Reg();
    ~Reg();
    
    int get_regs_index_value(unsigned int reg_index);
    int get_regs(unsigned int *reg, int len);
    int get_regs_len();
    int cmd_test_reg();
    
    int cpy_mem_to_file(char* dst_file, int offset, size_t total_len);
    int cpy_file_to_mem(char* src_file, int offset);
    int ice_server_watch_dog();
    
    int fpga_init(/*uint64_t mem_block_base, char* interrupt_path*/);
    int fpga_exit();
public:
    char* get_memBasseAddr() const { return m_memBaseAddr; }
    void* get_map_addr() const { return m_map_addr; } 
    void* get_map_addrCt() const { return m_map_addrCt; }
    volatile unsigned int* get_mmaped() const { return (m_mapped); }
private:
    
    const int WRITE_SIZE = 8 * 1024 * 1024;
    const int BUFFER_SIZE = 512 * 1024 * 1024;
    const int MAP_SIZE = 4095;

    char *m_memBaseAddr;
    void *m_map_addr;
    void *m_map_addrCt;
    //volatile unsigned int *m_mapped;
    unsigned int *m_mapped;
    int m_fd;
    int m_fd0;
    int m_mfd;
};



};// end of namespace snowlake
