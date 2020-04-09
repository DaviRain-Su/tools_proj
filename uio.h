#pragma once
#include <iostream>
#include <list>

#include <unistd.h>// sleep
#include <assert.h>//assert
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> // memset, memcpy
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <sys/mman.h>

#include "utils.h"
#include "Frame_head.h"

using std::list;

// read reg
#define ReadReg(BaseAddress, RegOffset) (*(volatile unsigned int *)((unsigned long)(BaseAddress) + RegOffset))
// write reg
#define WriteReg(BaseAddress, RegOffset, Data) *(volatile unsigned int *) ((unsigned long )(BaseAddress) + RegOffset) = Data

#define uio_print(fmt, ...) do { printf(GREEN"[%s]:"NONE fmt, __func__, ##__VA_ARGS__ ); }while(0)
#define uio_color_print(color, fmt, ...) do { printf(GREEN"[%s]:"color fmt NONE, __func__, ##__VA_ARGS__); }while(0)

class uio
{
    // new protocol
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
    uio();
    ~uio();


    int cpy_mem_to_file(char* dst_file, int offset, size_t total_len);
    int cpy_file_to_mem(char* src_file, int offset);
    
    int get_regs_index_value(unsigned int reg_index);
    int get_regs(unsigned int *reg, int len);
    int get_regs_len();
    int cmd_test_reg();

    int ice_server_watch_dog(void);
private:
    int fpga_init(/*uint64_t mem_block_base, char* interrupt_path*/);
    int fpga_exit();

private:
    const int WRITE_SIZE = 8 * 1024 * 1024; 
    const int BUFFER_SIZE  =  512 * 1024 * 1024;
    const int map_size = 4095;

    char* m_memBaseAddr; // 记录mem_base_addr
    void *map_addr;
    void *map_addrCt;
    volatile unsigned int *mapped;
    int fd;
    int fd0;
    int mfd;
};

uio::uio() {
   int ret =  fpga_init();
   if(ret == 1){
       uio_print("uio ctr fail \n");
   }else{
       uio_print("uio ctr success \n");
   }
}

uio::~uio(){
    int ret = fpga_exit();
    if(ret == 0){
        uio_print("exit success \n");
    }
}

int uio::cpy_mem_to_file(char* dst_file, int offset, size_t total_len){
    
    FILE *fp = fopen(dst_file, "w+");
    if(fp == NULL){
        uio_print("fail open file \n");
        return -1;
    }else{
        fseek(fp, 0, SEEK_SET);
        size_t write_size = fwrite(m_memBaseAddr + offset, 1, total_len, fp);

        if(write_size != total_len){
            uio_print("write size %zd fail \n", write_size);
        }else {
            uio_color_print(YELLOW, "Read  Data size 0x%zx, %zd \n", write_size, write_size);
        }
        sleep(1);
        uio_color_print(YELLOW, "write file %s over \n", dst_file);
        fclose(fp);
    }
    return 0;
}

int uio::cpy_file_to_mem(char* src_file, int offset){
    //assert(nname);
    if (!src_file) return -1; // nnam is NULL

    int ifd = open(src_file, O_RDWR);
    if (ifd < 0){
        uio_print("file don't exits ...\n");
        return -1; // ifd < 0 
    }
    
    const int BUF_SIZE = 512;

    int read_len = 0;
    int temp_total_len = 0;
    char rbuf[BUF_SIZE];

    char* p = (char*) malloc(512 * 512 * 3 * sizeof(char)); // ---

    lseek(ifd, 0, SEEK_SET);
    uio_print("read file starting ...\n");
    //可以不同方法改写读文件
    while( (read_len = read(ifd, rbuf, BUF_SIZE)) ) {
        // read file to buffer
        memcpy(p + temp_total_len, rbuf, read_len);
        temp_total_len += read_len;
        memset(rbuf, 0, BUF_SIZE);
    }

    // cpy p to m_memBaseAddr
    memcpy(m_memBaseAddr + offset, p, temp_total_len);

    uio_print("Write data over, src_file = %s, tmp_total_len = %d, 0x%x\n", src_file, temp_total_len, temp_total_len);

    memset(p, 0, 512 * 512 * 3 );
    free(p);
    p = nullptr;

    return 0;
}

int uio::get_regs_len(){
    char *ptr = (char*)mapped;
    return ptr[1];
}

int uio::get_regs_index_value(unsigned int reg_index) {
    int data_out;
    int ret = 0;
    char* ptr = (char*)mapped;
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
            int temp_1 = (mapped[mapped_index] >> shift_bits);
            int temp_2 = ((1 << (int_size - shift_bits)) - 1);
            int temp_3 = mapped[mapped_index + 1];
            int temp_4 = ((1 << (shift_bits + index_bits - int_size)) - 1);
            int temp_5 = (int_size - shift_bits);
            int temp_6 = (temp_3 & temp_4) << temp_5 ;
            index = ( temp_1 & temp_2) | temp_6;
        }else {// ---
            index = (mapped[mapped_index] >> shift_bits) & index_mask;
        }

        if(reg_index == index){
           data_out = mapped[1 + index_bytes + i]; // ---
           ret = data_out;
           break;
        }// end if
    }// end for

    return ret;
}

int uio::get_regs(unsigned *regs, int len) {
    if( (regs == NULL) || (len == 0) ) return 0;

    char *ptr = (char*) mapped;
    
    int index_bits = ptr[0];
    int index_num = ptr[1];
    int index_bytes = ptr[2];
    uio_print("index_bits = %d, index_num = %d, index_bytes = %d\n", index_bits, index_num, index_bytes);
    
    if(index_num != len) return 0;

    int index, shift_bits, mapped_index;
    unsigned int index_mask = (1 << index_bits) - 1;
    for(int i = 0; i < index_num; i++) {
        mapped_index = ((i * index_bits) / 32 ) + 1;
        shift_bits = (i * index_bits) % 32;
        
        if( 32 < shift_bits + index_bits){
            int temp1 = mapped[mapped_index] >> shift_bits;
            int temp2 = (1 << (32 - shift_bits)) - 1;
            int temp3 = (mapped[mapped_index + 1] & ((1 << (shift_bits + index_bits - 32)) - 1) );

            index = (temp1 &  temp2) | (temp3 << (32 - shift_bits));
        }else{
            index = (mapped[mapped_index] >> shift_bits) & index_mask;
        }
    }// end for
    return len;
}

int uio::cmd_test_reg()
{

    if(fd0 < 0){
        uio_print("Failed to open uio2 devfile\n");
        return Frame_head::ERR_STATUS::ERR_NORM;  
    }
    int inten = 1;
    write(fd0, &inten, 4);
    
    static bool fpga_lock = false;
    if(fpga_lock == false){
        usleep(10);
        usleep(10);
        mapped[uio::REG_INDEX0] = 0;
        usleep(10);
        mapped[uio::REG_INDEX0] = 1;
        usleep(10);
        fpga_lock = true;
    }else{
        uio_print("This Seq is wrong by the fpga running\n");
        return Frame_head::ERR_STATUS::ERR_BUSY;
    }

    int intcnt = 0;
    while(1) {
        read(fd0, &intcnt, 4); 
        uio_print("int cnt %d\n", intcnt);
        fpga_lock = false;
        break;
    }
    return Frame_head::ERR_STATUS::ERR_NONE;
}

int uio::ice_server_watch_dog(){
    int fd_watchdog = open("/dev/watchodg", O_WRONLY);
    int timeout = 10;
    
    static unsigned char food = 0;

    ioctl(fd_watchdog, WDIOC_SETTIMEOUT, &timeout); 
    
    while(1) {
        sleep(5);
        write(fd_watchdog, &food, 1);
    }
}

int uio::fpga_init(/*uint64_t mem_block_base, char *interrupt_path*/){
    uio_print("fpga_init begin ...!\n");

    if(map_size <= 0){
        fprintf(stderr, "Bad mapsize: %d\n", map_size);
        return 1;
    }

    fd = open("/dev/uio1", O_RDWR);
    if(fd < 0){
        perror("Failed to open uio1 devfile");
        return 1;
    }

    fd0 = open("/dev/uio2", O_RDWR);
    if(fd < 0) {
        perror("Failed to open uio2 devfile");
        return 1;
    }

    map_addr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( map_addr == MAP_FAILED){
        perror("Failed to mmap");
        return 1;
    }
    mapped = map_addr;

    mfd = open("/dev/mem", O_RDWR);
    
    map_addrCt = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE , MAP_SHARED, mfd, MEM_BLOCK_BASE);
    if(map_addrCt == MAP_FAILED){
        perror("Failed to mmap Higt memory");
        return 1;
    }

    m_memBaseAddr = (char*)map_addrCt;
    uio_print("fpga_init done ...!\n");
    uio_print("m_memBaseAddr = 0x%llx\n", m_memBaseAddr);
    return 0;
}

int uio::fpga_exit() {
    uio_print("--------------------\n");
    uio_print("fpga_ Exiting \n");
    uio_print("---------------------\n");
    
    munmap(map_addr, map_size);
    munmap(map_addrCt, BUFFER_SIZE);
    
    m_memBaseAddr = nullptr;
    map_addr = nullptr;
    map_addrCt = nullptr;
    mapped = nullptr;

    close(fd);
    close(fd0);
    close(mfd);
    return 0;
}




