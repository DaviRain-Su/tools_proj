#include "../inc/Message.h"
#include "../inc/utils.h"

namespace  snowlake {

Message::Message()
    : preamble_(Frame_preamble),
    len_(0),
    version_(Frame_version),//默认时0
    total_package_len_(0),
    seq_(-1),//默认是-1
    flag_(Frame_flag),//默认是0
    end_sym_(Frame_end_sym)
{
    checksum_.resize(Frame_checksum_size);
}

Message::Message(uint32_t len, uint32_t seq, const string &payload, uint32_t checksum)
    : preamble_(Frame_preamble),
    len_(len),
    version_(Frame_version),
    total_package_len_(0),
    seq_(seq),
    flag_(Frame_flag),
    checksum_(checksum),
    end_sym_(Frame_end_sym)
{
    payload_.insert(payload_.begin(), payload.begin(), payload.end());
    checksum_.resize(Frame_checksum_size);
}

void Message::set_preamble(uint32_t preamble)
{
    preamble_ = preamble;
}

void Message::set_len(uint32_t len)
{
    len_ = len;
}

void Message::set_version(uint32_t ver)
{
    version_ = ver;
}

void Message::set_total_package_len(uint32_t len)
{
    total_package_len_ = len;
}

void Message::set_seq(uint16_t seq)
{
    seq_ = seq;
}

void Message::set_flag(uint16_t flag)
{
    flag_ = flag;
}

void Message::set_payload(const string & lhs)
{
    payload_.insert(payload_.begin(), lhs.begin(), lhs.end());
}

void Message::set_payload(const char& lhs)
{
    payload_.push_back(lhs);
}

void Message::set_checksum(const string& checksum)
{
    checksum_.insert(checksum_.begin(), checksum.begin(), checksum.end());
}

void Message::set_end_sym(const uint8_t sym)
{
    end_sym_ = sym;
}

uint32_t Message::get_preamble() const 
{
    return preamble_;
}

uint32_t Message::get_len() const 
{
    return len_;
}

uint32_t Message::get_version() const 
{
    return version_;
}

uint32_t Message::get_total_package_len() const 
{
    return total_package_len_;
}

uint16_t Message::get_seq() const 
{
    return seq_;
}

uint16_t Message::get_flag() const 
{
    return flag_;
}

vector<char>& Message::get_payload() 
{
    return payload_;
}

vector<char> Message::get_payload() const 
{
    return payload_;
}

vector<char> Message::get_checksum() const 
{
    return checksum_;
}

uint8_t Message::get_end_sym() const 
{
    return end_sym_;
}

uint8_t Message::get_cmd() const 
{
    uint8_t cmd = 0; 
    if(payload_.size() == 0 ){
        return cmd;
    }
    // 需要注意的时cmd的长度设置的大小时uint8_t ，因此时一个字节的大小
    memcpy(&cmd, payload_.data(), sizeof(uint8_t));
    DEBUG(">> cmd is %d\n", cmd);
    return cmd;
}

void Message::set_cmd(uint8_t cmd) 
{
    DEBUG(">> the present payload size is %d\n", payload_.size());
    payload_.resize(sizeof(uint8_t));
    memcpy(payload_.data(), &cmd, sizeof(uint8_t));
    DEBUG(">> the present payload size is %d\n", payload_.size());
}

void Message::set_real_payload(const string &lhs)
{
#if 0
    payload_.reserve(payload_.size() + lhs.size());
    memcpy(payload_.data() + sizeof(uint8_t), lhs.data(), lhs.size());
#else
    DEBUG(">> the present payload size is %d\n", payload_.size());
    payload_.resize(payload_.size() + lhs.size());
    memcpy(payload_.data() + sizeof(uint8_t), lhs.data(), lhs.size());
    DEBUG(">> the present payload size is %d\n", payload_.size());
#endif
}

vector<char> && Message::get_real_payload() const 
{
    vector<char> temp;
    //从cmd 的后面开始提取真实的数据
    //cmd 的大小时uint8_t
    temp.insert(temp.begin(), payload_.begin() + sizeof(uint8_t), payload_.end());
    if(temp.size() == payload_.size() -1){
        DEBUG(">> Get real payload %d \n", temp.size());
    }else {
        DEBUG(">> not Get real payload \n");
    }
    return std::move(temp);
}

//用于求md5码
string && Message::get_Message_string() 
{

    int SIZE = get_Message_size();
    DEBUG(">> Message size is %d\n", get_Message_size());
    vector<char> tempStr(SIZE); // 必须要进行容器内存的初始化
    DEBUG(">> tempStr size is %d\n", tempStr.size());

    //set tempstr size
    int position = 0;

    //set preamble 
    uint32_t preamble_tmp = get_preamble();
    DEBUG(">> The current position is %d\n", position);
    DEBUG(">> the preamble is %d\n", preamble_tmp);
    memcpy(tempStr.data() + position, &preamble_tmp, sizeof(get_preamble()));

    //set len 
    position += sizeof(get_preamble());
    DEBUG(">> The current position is %d\n", position);
    uint32_t len_tmp = get_len();
    DEBUG(">> The len is %d\n", len_tmp);
    memcpy(tempStr.data() + position, &len_tmp, sizeof(get_len()));

    //set version
    position += sizeof(get_len());
    DEBUG(">> The current position is %d\n", position);
    uint32_t ver_tmp = get_version();
    DEBUG(">> The ver is %d\n", ver_tmp);
    memcpy(tempStr.data() + position, &ver_tmp, sizeof(get_version()));

    //set total_package_len --协议新添加的部分
    position += sizeof(get_version());
    DEBUG(">> The current position is %d\n", position);
    uint32_t total_package_len = get_total_package_len();
    DEBUG(">> The total_package_len is %d\n", total_package_len);
    memcpy(tempStr.data() + position, &total_package_len, sizeof(get_total_package_len()));

    //set seq
    position += sizeof(get_total_package_len());
    DEBUG(">> The current position is %d\n", position);
    uint16_t seq_tmp = get_seq();
    DEBUG(">> The seq is %d\n", seq_tmp);
    memcpy(tempStr.data() + position, &seq_tmp, sizeof(get_seq()));

    //set flags
    position += sizeof(get_seq());
    DEBUG(">> The current position is %d\n", position);
    uint16_t flag_tmp = get_flag();
    DEBUG(">> The flag is %d\n", flag_tmp);
    memcpy(tempStr.data() + position, &flag_tmp, sizeof(get_flag()));

    //set payload
    position += sizeof(get_flag());
    DEBUG(">> The current position is %d\n", position);
    DEBUG(">> payload size is %d\n", payload_.size());
    tempStr.insert(tempStr.begin() + position, payload_.begin(), payload_.end());
    DEBUG(">> payload size is %d\n", payload_.size());
    
    // return the Message String
    return std::move(string(tempStr.begin(), tempStr.end()));
}

int Message::get_preamble_size() const 
{
    return sizeof(preamble_);
}

int Message::get_len_size() const 
{
    return sizeof(len_);
}

int Message::get_version_size() const 
{
    return sizeof(version_);
}

int Message::get_total_package_len_size() const 
{
    return sizeof(total_package_len_);
}

int Message::get_seq_size() const 
{
    return sizeof(seq_);
}

int Message::get_flag_size() const 
{
    return sizeof(flag_);
}

int Message::get_cmd_status_size() const 
{
    return Cmd_status_len;
}

int Message::get_err_status_size() const 
{
    return Err_status_len;
}

int Message::get_end_sym_size() const 
{
    return End_sym_len;
}

int Message::get_myconfig_size() const 
{
    return Myconfig_max_len;
}

int Message::get_checksum_size() const 
{
    return Frame_checksum_size;
}

void Message::to_string() 
{
#if 0
#else
        DEBUG("--------------------------------\n");
        DEBUG(">> preamble --> %d \n", preamble_);
        DEBUG(">> len --> %d \n", len_);
        DEBUG(">> version --> %d \n", version_);
        DEBUG(">> total_package_len --> %d\n", total_package_len_);
        DEBUG(">> seq --> %d\n", seq_);
        DEBUG(">> flag --> %d\n", flag_);
        DEBUG(">> payload size is --> %d\n", payload_.size());
        DEBUG(">> cmd --> %d\n", get_cmd());
        vector<char> ret_payload = get_real_payload();
        DEBUG(">> real payload size is  --> %d\n", ret_payload.size());
        DEBUG(">> real payload is ---> %s\n", string(ret_payload.begin(), ret_payload.end()).data());
        DEBUG(">> checksum --> %s\n", get_checksum().data());
        DEBUG(">> end_sym --> %d\n", get_end_sym());
        DEBUG("--------------------------------\n");
#endif

}

int Message::get_Message_size() const 
{
    return sizeof(preamble_) 
        + sizeof(len_) 
        + sizeof(version_) 
        + sizeof(total_package_len_)
        + sizeof(seq_) 
        + sizeof(flag_) 
        + payload_.size(); 
}

int Message::get_full_package_size() const 
{
    return get_Message_size() + get_checksum_size() + get_end_sym_size();
}

}// end of namespace snowlake

