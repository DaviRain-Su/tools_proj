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

#define reg_print(fmt, ...) do { printf(GREEN"[%s]:"NONE fmt, __func__, ##__VA_ARGS__ ); }while(0)
#define reg_color_print(color, fmt, ...) do { printf(GREEN"[%s]:"color fmt NONE, __func__, ##__VA_ARGS__); }while(0)

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
private:
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
    volatile unsigned int *m_mapped;
    int m_fd;
    int m_fd0;
    int m_mfd;
};

Reg::Reg() {
   int ret =  fpga_init();
   if(ret == -1){
       reg_print("uio ctr fail \n");
   }else{
       reg_print("uio ctr success \n");
   }
}

Reg::~Reg(){
    int ret = fpga_exit();
    if(ret == 0){
        reg_print("exit success \n");
    }
}

int Reg::fpga_init(/*uint64_t mem_block_base, char *interrupt_path*/){
    reg_print("fpga_init begin ...!\n");

    if(MAP_SIZE <= 0){
        fprintf(stderr, "Bad mapsize: %d\n", MAP_SIZE);
        return -1;
    }


    m_fd0 = open("/dev/uio2", O_RDWR);
    if(m_fd0 < 0) {
        perror("Failed to open uio2 devfile");
        return -1;
    }

    m_fd = open("/dev/uio1", O_RDWR);
    if(m_fd < 0){
        perror("Failed to open uio1 devfile");
        return -1;
    }
    m_map_addr = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if(m_map_addr == MAP_FAILED){
        perror("Failed to mmap");
        return -1;
    }
    m_mapped = m_map_addr;

    m_mfd = open("/dev/mem", O_RDWR);
    if(m_mfd < 0){
        perror("Failed tp opem mem devfile");
        return -1;
    }
    m_map_addrCt = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE , MAP_SHARED, m_mfd, MEM_BLOCK_BASE);
    if(m_map_addrCt == MAP_FAILED){
        perror("Failed to mmap Higt memory");
        return -1;
    }

    m_memBaseAddr = (char*)m_map_addrCt;
    reg_print("fpga_init done ...!\n");
    reg_print("m_memBaseAddr = 0x%llx\n", m_memBaseAddr);
    return 0;
}

int Reg::fpga_exit() {
    reg_print("--------------------\n");
    reg_print("fpga_ Exiting \n");
    reg_print("---------------------\n");
    
    munmap(m_map_addr, MAP_SIZE);
    munmap(m_map_addrCt, BUFFER_SIZE);
    
    m_memBaseAddr = nullptr;
    m_map_addr = nullptr;
    m_map_addrCt = nullptr;
    m_mapped = nullptr;

    close(m_fd);
    close(m_fd0);
    close(m_mfd);
    return 0;
}

int Reg::get_regs_len(){
    char *ptr = (char*)get_mmaped();
    return ptr[1];
}

int Reg::get_regs_index_value(unsigned int reg_index) {
    int ret = 0;
    char* ptr = (char*)get_mmaped();

    const int int_size = sizeof(int) * 8; // -- 
    int index_bits = ptr[0]; // ---
    int index_num = get_regs_len();
    int index_bytes = ptr[2]; // ---

    unsigned int index_mask = (1 << index_bits) - 1;// --

    unsigned int index = 0, shift_bits = 0, mapped_index = 0;
    for(int i = 0; i < index_num; i++) {
        
        mapped_index = ((i * index_bits) / int_size ) + 1;// --

        shift_bits = (i * index_bits) % int_size;// ---

        if(int_size < shift_bits + index_bits){ // ----
            int temp_1 = (get_mmaped()[mapped_index] >> shift_bits);
            int temp_2 = ((1 << (int_size - shift_bits)) - 1);
            int temp_3 = get_mmaped()[mapped_index + 1];
            int temp_4 = ((1 << (shift_bits + index_bits - int_size)) - 1);
            int temp_5 = (int_size - shift_bits);
            int temp_6 = (temp_3 & temp_4) << temp_5 ;
            index = ( temp_1 & temp_2) | temp_6;
        }else {// ---
            index = (get_mmaped()[mapped_index] >> shift_bits) & index_mask;
        }

        if(reg_index == index){
           int data_out;
           data_out = get_mmaped()[1 + index_bytes + i]; // ---
           ret = data_out;
           break;
        }// end if
    }// end for

    return ret;
}

int Reg::get_regs(unsigned *regs, int len) {
    if( (regs == NULL) || (len == 0) ) return 0;

    char *ptr = (char*)get_mmaped();
    
    int index_bits = ptr[0];
    int index_num = ptr[1];
    int index_bytes = ptr[2];
    reg_print("index_bits = %d, index_num = %d, index_bytes = %d\n", index_bits, index_num, index_bytes);
    
    if(index_num != len) return 0;

    int index, shift_bits, mapped_index;
    unsigned int index_mask = (1 << index_bits) - 1;
    for(int i = 0; i < index_num; i++) {
        mapped_index = ((i * index_bits) / 32 ) + 1;
        shift_bits = (i * index_bits) % 32;
        
        if( 32 < shift_bits + index_bits){
            int temp1 = get_mmaped()[mapped_index] >> shift_bits;
            int temp2 = (1 << (32 - shift_bits)) - 1;
            int temp3 = (get_mmaped()[mapped_index + 1] & ((1 << (shift_bits + index_bits - 32)) - 1) );

            index = (temp1 &  temp2) | (temp3 << (32 - shift_bits));
        }else{
            index = (get_mmaped()[mapped_index] >> shift_bits) & index_mask;
        }
    }// end for
    return len;
}

int Reg::cmd_test_reg()
{

    if(m_fd0 < 0){
        reg_print("Failed to open uio2 devfile\n");
        return Frame_head::ERR_STATUS::ERR_NORM;  
    }
    int inten = 1;
    write(m_fd0, &inten, 4);
    
    static bool fpga_lock = false;
    if(fpga_lock == false){
        usleep(10);
        usleep(10);
        get_mmaped()[REG_INDEX0] = 0;
        usleep(10);
        get_mmaped()[REG_INDEX0] = 1;
        usleep(10);
        fpga_lock = true;
    }else{
        reg_print("This Seq is wrong by the fpga running\n");
        return Frame_head::ERR_STATUS::ERR_BUSY;
    }

    int intcnt = 0;
    while(1) {
        read(m_fd0, &intcnt, 4); 
        reg_print("int cnt %d\n", intcnt);
        fpga_lock = false;
        break;
    }
    return Frame_head::ERR_STATUS::ERR_NONE;
}

int Reg::cpy_mem_to_file(char* dst_file, int offset, size_t total_len){
    
    FILE *fp = fopen(dst_file, "w+");
    if(fp == NULL){
        reg_print("fail open file \n");
        return -1;
    }else{
        fseek(fp, 0, SEEK_SET);
        size_t write_size = fwrite(m_memBaseAddr + offset, 1, total_len, fp);

        if(write_size != total_len){
            reg_print("write size %zd fail \n", write_size);
        }else {
            reg_color_print(YELLOW, "Read  Data size 0x%zx, %zd \n", write_size, write_size);
        }
        sleep(1);
        reg_color_print(YELLOW, "write file %s over \n", dst_file);
        fclose(fp);
    }
    return 0;
}

int Reg::cpy_file_to_mem(char* src_file, int offset){
    //assert(nname);
    if (!src_file) return -1; // nnam is NULL

    int ifd = open(src_file, O_RDWR);
    if (ifd < 0){
        reg_print("file don't exits ...\n");
        return -1; // ifd < 0 
    }
    
    const int BUF_SIZE = 512;

    int read_len = 0;
    int temp_total_len = 0;
    char rbuf[BUF_SIZE];

    char* temp_p = (char*) malloc(512 * 512 * 3 * sizeof(char)); // ---

    lseek(ifd, 0, SEEK_SET);
    reg_print("read file starting ...\n");
    //可以不同方法改写读文件
    while( (read_len = read(ifd, rbuf, BUF_SIZE)) ) {
        // read file to buffer
        memcpy(temp_p + temp_total_len, rbuf, read_len);
        temp_total_len += read_len;
        memset(rbuf, 0, BUF_SIZE);
    }

    // cpy p to m_memBaseAddr
    memcpy(m_memBaseAddr + offset, temp_p, temp_total_len);

    reg_print("Write data over, src_file = %s, tmp_total_len = %d, 0x%x\n", src_file, temp_total_len, temp_total_len);

    memset(temp_p, 0, 512 * 512 * 3 );
    free(temp_p);
    temp_p = nullptr;

    return 0;
}


int Reg::ice_server_watch_dog(){
    int fd_watchdog = open("/dev/watchodg", O_WRONLY);
    int timeout = 10;
    
    static unsigned char food = 0;

    ioctl(fd_watchdog, WDIOC_SETTIMEOUT, &timeout); 
    
    while(1) {
        sleep(5);
        write(fd_watchdog, &food, 1);
    }
    return 0;
}


};// namespace snowlake
