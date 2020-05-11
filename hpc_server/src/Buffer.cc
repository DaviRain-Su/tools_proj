#include "../inc/Buffer.h"
#include "../inc/utils.h"

namespace  snowlake {

Buffer::Buffer()
    : _realSize(0)
{
    cout << "Buffer()" << endl;        
}

Buffer::Buffer(int size)
    : _realSize(0)
{
    buff_.resize(size);
}

string &&  Buffer::encode(const Message &lhs) 
{   // from Message --> buff
    // push _buff
    //将数据进行encode

    int position = 0;
    DEBUG(">> The current position preamble is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    
    //set preamble 
    uint32_t preamble_tmp = lhs.get_preamble();
    DEBUG(">> preamble is %d\n", preamble_tmp);
    memcpy(buff_.data() + position, &preamble_tmp, lhs.get_preamble_size());

    //set len 
    position += lhs.get_preamble_size();
    DEBUG(">> The current position len is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint32_t len_tmp = lhs.get_len();
    DEBUG(">> len is %d\n", len_tmp);
    memcpy(buff_.data() + position, &len_tmp, lhs.get_len_size());

    //set version
    position += lhs.get_len_size();
    DEBUG(">> The current position version is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint32_t ver_tmp = lhs.get_version();
    DEBUG(">> ver is %d\n", ver_tmp);
    memcpy(buff_.data() + position, &ver_tmp, lhs.get_version_size());

    //set total_package_len 
    position += lhs.get_version_size();
    DEBUG(">> The current position total_package_len is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint32_t total_package_len = lhs.get_total_package_len();
    DEBUG(">> total_package_len is %d\n", total_package_len);
    memcpy(buff_.data() + position, &total_package_len, lhs.get_total_package_len_size());

    //set seq
    position += lhs.get_total_package_len_size();
    DEBUG(">> The current position  seq is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint16_t seq_tmp = lhs.get_seq();
    DEBUG(">> seq is %d\n", seq_tmp);
    memcpy(buff_.data() + position, &seq_tmp,lhs.get_seq_size());

    //set flags
    position += sizeof(lhs.get_seq());
    DEBUG(">> The current position flag is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint16_t flag_tmp = lhs.get_flag();
    DEBUG(">> flag is %d\n", flag_tmp);
    memcpy(buff_.data() + position, &flag_tmp, lhs.get_flag_size());

    //set payload
    position += lhs.get_flag_size();
    DEBUG(">> The current position payload is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    vector<char> temp_payload = lhs.get_payload();
    DEBUG(">> payload size is %d\n", temp_payload.size());
    memcpy(buff_.data() + position, temp_payload.data(), temp_payload.size());

    // set check sum
    position += temp_payload.size();
    DEBUG(">> The current position checksum is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    vector<char> checksum_tmp = lhs.get_checksum();
    DEBUG(">> checksum is %s\n", checksum_tmp.data());
    memcpy(buff_.data() + position, checksum_tmp.data(), lhs.get_checksum_size());

    // set end sym
    position += lhs.get_checksum_size();
    DEBUG(">> The current position end sum is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    uint8_t end_sym_tmp = lhs.get_end_sym();
    DEBUG(">> end_sym is %d\n", end_sym_tmp);
    memcpy(buff_.data() + position, &end_sym_tmp, lhs.get_end_sym_size()); 

    position += lhs.get_end_sym_size();
    DEBUG(">> The current position total  is %d\n", position);
    DEBUG(">> _buff size is %d\n", buff_.size());
    _realSize = position;//set the present size
    DEBUG(">> _buff size is %d\n", buff_.size());
    if(_realSize < SIZE){
        cout << "set buff is true !" << endl;
    }else {
        cout << "set buff is false !" << endl; 
    }

    //return the encode buff
    return std::move(string(buff_.begin(), buff_.end()));
}

Message && Buffer::decode(const string &lhs)
{   
    // form buff --> Message
    DEBUG(">> the get string size is %d\n", lhs.size());
#if 0
    Pkt_head *head = (Pkt_head*)lhs.data();
    if(!fream_head_is_vaild(head)){
        cout << ">> this msg is ivaild !!!!" << endl;
        return std::move(Message());//出错返回一个默认的Message
    }
#endif
    //返回解码后的结构体数据
    Message tempMsg;

    int position = 0;
    DEBUG(">> The current position is %d\n", position);
    //将lhs 的前四个的字节写入到 preamble
   
    //set preamble
    uint32_t preamble = 0;
    memcpy(&preamble, lhs.data() + position, tempMsg.get_preamble_size());
    DEBUG(">> the preamble is %d\n", preamble);
    tempMsg.set_preamble(preamble);    

    //set len
    position += sizeof(tempMsg.get_preamble());
    DEBUG(">> The current position is %d\n", position);
    uint32_t len = 0;
    memcpy(&len, lhs.data() + position, tempMsg.get_len_size());
    DEBUG(">> the len is %d\n", len);
    tempMsg.set_len(len);

    //set version
    position += sizeof(tempMsg.get_len());
    DEBUG(">> The current position is %d\n", position);
    uint32_t ver = 0;
    memcpy(&ver, lhs.data() + position, tempMsg.get_version_size());
    DEBUG(">> the ver is %d\n", ver);
    tempMsg.set_version(ver);

    //set total_package_len -- 协议新添加部分
    position += sizeof(tempMsg.get_version());
    DEBUG(">> The current position is %d\n", position);
    uint32_t total_package_len = 0;
    memcpy(&total_package_len, lhs.data() + position, tempMsg.get_total_package_len_size());
    DEBUG(">> the total_package_len is %d\n", total_package_len);
    tempMsg.set_total_package_len(total_package_len);

    //set seq 
    position += sizeof(tempMsg.get_total_package_len());
    DEBUG(">> The current position is %d\n", position);
    uint16_t seq = 0;
    memcpy(&seq, lhs.data() + position, tempMsg.get_seq_size());
    DEBUG(">> the seq is %d\n", seq);
    tempMsg.set_seq(seq);

    //set flag
    position += sizeof(tempMsg.get_seq());
    DEBUG(">> The current position is %d\n", position);
    uint16_t flag = 0;
    memcpy(&flag, lhs.data() + position, tempMsg.get_flag_size());
    DEBUG(">> the flag is %d\n", flag);
    tempMsg.set_flag(flag);

    //set payload
    position += sizeof(tempMsg.get_flag());
    DEBUG(">> The current position is %d\n", position);
    //得到payload的起始的位置
    int back_position = lhs.size() - sizeof(tempMsg.get_end_sym()) - sizeof(tempMsg.get_checksum().size());
    //get payload , 截取payload 在 lhs中
    string payload(lhs, position,  back_position);//set payload
    DEBUG(">> the payload is %d\n", payload.data());
    //设置payload
    tempMsg.set_payload(payload);

    //set check sum
    //check sum size is 32
    position = lhs.size() - tempMsg.get_checksum_size() - tempMsg.get_end_sym_size();
    DEBUG(">> The current position is %d\n", position);
    string checksum(lhs, position, position + tempMsg.get_checksum_size());
    DEBUG(">> the checksum is %s\n", checksum.data());
    //设置checksum
    tempMsg.set_checksum(checksum);

    //set end sym
    position = lhs.size() - tempMsg.get_end_sym_size();
    DEBUG(">> The current position is %d\n", position);
    uint8_t end_sym = 0;
    memcpy(&end_sym, lhs.data() + position, sizeof(tempMsg.get_end_sym()));
    DEBUG(">> the end sym is %d\n", end_sym);
    //设置end_sym
    tempMsg.set_end_sym(end_sym);

    position += sizeof(tempMsg.get_end_sym());
    DEBUG(">> The current position is %d\n", position);
    if(position == lhs.size())
    {
        cout << "decode successful !!" << endl;
    }else {
        cout << "decode failded !!" << endl;
    }

    return std::move(tempMsg);
}

vector<char>& Buffer::getBuff() 
{
    //  for  send buff
    return buff_;
}

int Buffer::get_realSize() const  
{ 
    // for send buff
    return _realSize;
}

bool Buffer::fream_head_is_vaild(Pkt_head *pkt_head)
{
    if(pkt_head->preamble != Frame_preamble){
        return false;

    }else if (pkt_head->len < Frame_min_len || Frame_max_len < pkt_head->len)
    {
        return false;

    }

    // check sum 忽略
    
    return true;
}
}
