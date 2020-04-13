#include "../inc/Frame_head.h"
namespace  snowlake { // namespace snowlake

pkt_head_t::pkt_head_t() :
    m_preamble(FRAME_PREAMBLE)
    , m_len(0)
    , m_ver(0)
    , m_seq(0)
    , m_flag(0)
{ // 默认前导符
    std::cout << "pkt_head_t()" << std::endl;
}

void pkt_head_t::set_pkt_head(uint32_t len, uint16_t seq, uint16_t flags){
    m_len = len;
    m_seq = seq;
    m_flag = flags;
    m_ver = 1;
}

uint32_t pkt_head_t::get_preamble() const {
    return m_preamble;
}

uint32_t pkt_head_t::get_len() const { 
    return m_len; 
}

uint16_t pkt_head_t::get_seq() const { 
    return m_seq; 
}

uint16_t pkt_head_t::get_flag() const { 
    return m_flag; 
}

unsigned int pkt_head_t::get_pkt_data_len() const {
    return m_len - FRAME_HEAD_LEN - 5;
}

void pkt_head_t::set_preamble(uint32_t lhs) { 
    m_preamble = lhs;
}

Frame_head::Frame_head() {
    std::cout << "Frame_head()" << std::endl;
}


Frame_head::~Frame_head() {
    free(m_f_head);
    m_f_head = nullptr;
    std::cout << "~Frame_head()" << std::endl;
}

void Frame_head::set_frame_head(F_Head* phead){
    m_f_head = phead;
}

//包头长度，目前16字节
size_t Frame_head::get_frame_head_len() const { 
    return FRAME_HEAD_LEN;
}
// 最小包长度
size_t Frame_head::get_frame_min_len() const { 
    return FRAME_MIN_LEN;
}
//最大包长度
size_t Frame_head::get_frame_max_len() const { 
    return FRAME_MAX_LEN;
}

size_t Frame_head::get_body_info_len() const  { 
    return BODY_INFO_LEN;
}
size_t Frame_head::get_err_status_len() const { 
    return ERR_STATUS_LEN;
}
size_t Frame_head::get_cmd_status_len() const {
    return CMD_STATUS_LEN;
}
size_t Frame_head::get_end_data_len() const { 
    return END_DATA_LEN;
}
size_t Frame_head::get_check_sum_len() const { 
    return CHECK_SUM_LEN;
}
size_t Frame_head::get_status_info_len() const { 
    return STATUS_INFO_LEN;
}
uint32_t Frame_head::get_frame_head_preamble() const  { 
    return FRAME_PREAMBLE;
}
bool Frame_head::freamehead_is_vaild(){
    if( m_f_head->get_preamble() != get_frame_head_preamble()){
        return false;
    }
    if(m_f_head->get_len() < get_frame_min_len() || m_f_head->get_len() > get_frame_max_len()){
        return false;
    }
    if( check_sum(&m_f_head, 0xffffffff, m_f_head->get_len()) != 0 ){
        return false;
    }
    return true;
}
// 包长度
unsigned int Frame_head::get_frame_total_len(){ 
    return m_f_head->get_len();
}
// 数据部分长度

unsigned int Frame_head::get_frame_data_len() {
    return m_f_head->get_pkt_data_len();
}

void Frame_head::frame_fill_head(uint32_t len, uint16_t seq, uint16_t flags){
    m_f_head->set_pkt_head(len, seq, flags);
}


}; // end of namespace snowlake
