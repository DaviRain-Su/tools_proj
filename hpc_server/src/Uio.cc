#include "../inc/Uio.h"

namespace  snowlake {
//
//@para[name] copy mem to file 
//@para[1] out　file name 
//@para[2] offset, mem_base_offset
//@para[3] copy data len 
//
//
void Uio::copy_mem_to_file(const string &outfilename, int offset, int total_len)
{
    FILE *fp = fopen(outfilename.data(), "w+");
    if(fp!=0){
        fseek(fp, 0, SEEK_SET);
        char *current_pos = _memBaseAddr + offset;
        size_t write_size = fwrite((const void*)current_pos, 1, total_len, fp);
        if(write_size != total_len){
            std::cout << "Write size " << write_size << "fail" << std::endl;
        }else {
            std::cout << "Read Data size  is  " << write_size << std::endl;
        }
        sleep(1);
        std::cout << "write file " << outfilename << " over" << std::endl;
    }
    fclose(fp);
}

//
//@para[name] copy file to mem
//@para[1] in file name , output file to mem_base_offset
//@para[2] offset, this is a offset to mem_base_addr
//
void Uio::copy_file_to_mem(const string &infilename, int offset) 
{
    assert(infilename.size() != 0);

    int fd = 0;
    fd = open(infilename.data(), O_RDWR);
    if(fd < 0){
        std::cout << "file don't exit ..." << std::endl;
        return;
    }

    int read_len = 0;
    int temp_total_len = 0;
    char temp_buf[SIZE] = {0};

    char*p = (char*)malloc(IMAGE_SIZE * sizeof(char));//后期可能会变更
    lseek(fd, 0, SEEK_SET);

    std::cout << ">> Read file starting ..." << std::endl;
    while( (read_len = read(fd, temp_buf, SIZE)) )
    {
        memcpy(p + temp_total_len, temp_buf, read_len);
        temp_total_len += read_len;
        memset(temp_buf, 0, SIZE);
    }
    //将文文件全部读取到memBaseAddr
    memcpy(_memBaseAddr + offset, p, temp_total_len);

    std::cout << "Write data over, src_file " << infilename << " temp_total_len = "
        << temp_total_len << std::endl;

    memset(p, 0, IMAGE_SIZE);
    free(p);
    return;
}

//
//get reg len
//
int Uio::get_reg_len()
{
    char *ptr = (char*)_mapped;
    return  ptr[1];
}

//
//get regs index value
//
int Uio::get_regs_index_value(unsigned int reg_index)
{

    int data_out;
    int ret = 0;
    char* ptr = (char*)_mapped;

    const int int_size = sizeof(int) * 8;

    int index_bits  = ptr[0];
    int index_num = ptr[1];
    int inedx_bytes = ptr[2];

    unsigned int index_mask = (1 << index_bits ) - 1;

    int i, index, shift_bits, mapped_index;

    for(i = 0; i < index_num; i++){

        mapped_index = ((i * index_bits) / int_size ) + 1;
        shift_bits  = (i * index_bits ) % int_size;

        if(shift_bits + index_bits > int_size){
            int temp0 = _mapped[mapped_index] >> shift_bits ;
            int temp1 = (1 << (int_size - shift_bits)) - 1;
            int temp2 = temp0 & temp1;
            int temp3 = _mapped[mapped_index + 1] & ((1 << (shift_bits + index_bits - int_size)) - 1);
            int temp4 = temp3 << (int_size - shift_bits);
            index = temp2 | temp4;
        }else {
            index = (_mapped[mapped_index] >> shift_bits) & index_mask;
        }

        if(reg_index == index){
            data_out = _mapped[1 + inedx_bytes + i];
            ret  = data_out;
            break;
        }// end if
    }// end for

    return ret;
}

//
//get regs
//
int Uio::get_regs(unsigned int *regs, int len) 
{
    if(regs == nullptr || len == 0)
        return 0;

    char *ptr = (char*)_mapped;

    int index_num = ptr[1];

    if(index_num != len)
        return 0;
    return len;
}

//
//cmd test reg
//
int Uio::cmd_test_reg()
{
    int intcnt =  0;
    int inten = 1;
    //    void* map_addr, *map_addr1;
    static bool fpga_lock = false;

    if(_fd0 < 0){
        std::cout << "Failed to open uio2 dev file " << std::endl;
        return -1; // ERR_NORM
    }

    write(_fd0, &inten , 4);

    if(fpga_lock == false){

        usleep(10);

        usleep(10);

        _mapped[REG_INDEX0] = 0;

        usleep(10);

        _mapped[REG_INDEX0] = 1;

        usleep(10);

        fpga_lock = true;
    }else {

        std::cout << "This seq is wrong by the fpga running " << std::endl;

        return -1; // ERR_BUSY
    }


    while(true){

        read(_fd0, &intcnt, 4);

        std::cout << "int cnt " << intcnt << std::endl;

        fpga_lock = false;

        break;
    }
    return  0; // ERR_NONE
}

// 
//fpga init
//
int Uio::fpga_init(uint64_t mem_block_base, const string & interrupt_path)
{
    // mem_block_base, interrupt_path 没有用到
    std::cout << "fpga_init begin ... !" << std::endl;

    if(MAP_SIZE <= 0){
        // mapsize = 4095
        std::cout << "Bad mapsize " << MAP_SIZE << std::endl;;
        return  1;
    }

    _fd = open("/dev/uio1", O_RDWR);
    if(_fd < 0){
        std::cout << "Failed to open uio1 dev file " << std::endl;
        return 1;
    } 

    _fd0 = open("/dev/uio2", O_RDWR);
    if(_fd0 < 0){
        std::cout << "Failed to open uio2 dev file " << std::endl;
        return 1;
    }

    _map_addr = mmap(nullptr, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0 );
    if(_map_addr == MAP_FAILED){
        std::cout << "Failed to mmap" << std::endl;
        return 1;
    }

    _mapped = (unsigned int*)_map_addr;

    _mfd = open("/dev/mem", O_RDWR);
    if(_mfd < 0){
        std::cout << "Failed to open mem dev file " << std::endl;
        return 1;
    }

    _map_addrCt = mmap(nullptr,
                       BUFFER_SIZE,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       _mfd,
                       MEM_BLOCK_BASE); // MEM_BLOCK_BASE
    if(_map_addrCt == MAP_FAILED){
        std::cout << "Failed to mmmap high memory " << std::endl;
        return 1;
    }

    _memBaseAddr = (char*)_mapAddrCt;
    std::cout << "Fpga_init done ... !" << std::endl;
    std::cout << "_memBaseAddr  = " << _memBaseAddr << std::endl;

    return 0;
}

//
//fpga init
//
int Uio::fpga_exit()
{
    std::cout << "---------------" << std::endl;
    std::cout << "Fpga Exiting " << std::endl;
    std::cout << "---------------" << std::endl;

    munmap(_map_addr, MAP_SIZE);
    munmap(_map_addrCt, BUFFER_SIZE);

    close(_fd);
    close(_fd0);
    close(_mfd);

    return 0;
}

}// end of namespace snowlake
