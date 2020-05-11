#include "../inc/client.h"

namespace snowlake{// start of namespace snowlake

int Client_info::_index = 0;
Client_info::Client_info()
    :_fd(0)
     , _rxlen(0)
     , _rxtime(0)
     ,_fsmstate(0)
{
    memset(_rxbuf, 0, FRAME_MAX_LEN);
    _index++;
    std::cout << "Clinet_info()" << std::endl;
    std::cout << "index is " << _index << std::endl;
}

Client_info::Client_info(int fd, unsigned char *rxbuf, int rxlen, time_t rxtime,int fsmstate)
    : _fd(fd)
    , _rxlen(rxlen)
    , _rxtime(rxtime)
    , _fsmstate(fsmstate)
{
    if(rxbuf){
        memcpy(_rxbuf, rxbuf, strlen((char*)rxbuf));
    }else{
        memset(_rxbuf, 0, sizeof(_rxbuf));
    }
    _index++;
    std::cout << "Clinet_info(int, unsigned char*, int, time_t, int)" << std::endl;
    std::cout << "index is " << _index << std::endl;
}
void Client_info::set_client_info_fd(int fd) {
    _fd = fd;
}
void Client_info::set_client_info_len(int len) {
    _rxlen = len;
}
void Client_info::set_client_info_time(time_t t){
    _rxtime = t;
}
void Client_info::set_client_info_state(int state){
    _fsmstate = state;
}
Client_info::~Client_info () {
    std::cout << "~Clinet_info()" << std::endl;
}
// --------------------------------------------------------------------------------------
client::client()
    : ptr_protocol(new Protocol())
{
    //m_clients.push_back(Client_info());

    std::cout << "client ()" << std::endl;
}

client::~client() {
    m_clients.erase(m_clients.begin(), m_clients.end());
    std::cout << "client number is " << m_clients.size() << std::endl;
    std::cout << "~client()" << std::endl;
}

void client::clients_itor(Callback fn, void *args){
    for(auto iter = m_clients.begin(); iter != m_clients.end(); iter++){
        fn(*iter, args);
    }    
}
void client::clients_itor_add_to_set(void *args){
    for(auto iter = m_clients.begin(); iter != m_clients.end(); iter++) {
        client_add_to_set(*iter, args);
    }
}
void client::clients_itor_read_process(void *args){
    for(auto iter = m_clients.begin(); iter != m_clients.end(); iter++){
        client_read_process(*iter, args);
    }    
}
int client::client_add_to_set(Client_info &pinfo, void *args){
    FD_SET(pinfo._fd, (fd_set *)args);
    return 0;
}
void client::client_info_free(){
    m_clients.erase(m_clients.begin(), m_clients.end());
}

int client::client_read_process(Client_info &pinfo, void *args){

    int recv_len;
    static int index = 0;
    time_t now;

    int nread;

    time(&now);

    if(FD_ISSET(pinfo._fd, (fd_set*)args)){

        F_Head* phead = (F_Head*)pinfo._rxbuf;
        int result_len = phead->get_len() - pinfo._rxlen;
        uint64_t result = pinfo._fsmstate == Frame_head::RX_STATE::RX_STATE_START ? FRAME_HEAD_LEN : result_len; 
        recv_len = recv(pinfo._fd, 
                        pinfo._rxbuf + pinfo._rxlen,
                        result,
                        0);
        index += recv_len;

        if(0 < recv_len){
            pinfo._rxlen += recv_len;
            pinfo._rxtime = now;

            switch(pinfo._fsmstate){
            case Frame_head::RX_STATE::RX_STATE_START:
                DBG_INFO("get frame head ... \r\n");
                if(FRAME_HEAD_LEN <= pinfo._rxlen){
                    Frame_head temp;
                    temp.set_frame_head(phead);
                    if(temp.freamehead_is_vaild()){
                        pinfo._fsmstate = Frame_head::RX_STATE::RX_STATE_HEAD_GOT;
                    }else{
                        DBG_INFO("Error data head got \r\n");
                        pinfo._rxlen = 0;
                    }
                }
                break;
            case Frame_head::RX_STATE::RX_STATE_HEAD_GOT:
                if(phead->get_len() <= pinfo._rxlen){
                    pinfo._fsmstate = Frame_head::RX_STATE::RX_STATE_START;

                    if(FRAME_END_SYM != pinfo._rxbuf[phead->get_len() -1 ]){
                        DBG_INFO("Error data end got\r\n");
                        pinfo._rxlen = 0;
                    }else{
                        DBG_INFO("pinfo.rxbuf %d\n", index);
                        DBG_INFO("....................................\n");
                        index = 0;
                        if(ptr_protocol->cmd_process(pinfo._fd, pinfo._rxbuf, phead->get_len()) != 0){
                            DBG_INFO("CMD ERROR, CLOSE SOCKET \n");
                            goto error_process; 
                        }
                        pinfo._rxlen = 0;
                    }
                }

                break;
            default:
                DBG_INFO("Error RX State\r\n");
                pinfo._fsmstate = Frame_head::RX_STATE::RX_STATE_START;
                pinfo._rxlen = 0;
                goto  error_process;
            }
        }else{
            DBG_INFO("Error RX State \r\n");
            FD_CLR(pinfo._fd, (fd_set*)args);
            close(pinfo._fd);
            client_info_free(); 
        }
    }else if( 60 < now - pinfo._rxtime ){
        DBG_INFO("socket %d timeout \n", pinfo._fd);
error_process:
        FD_CLR(pinfo._fd, (fd_set*)args);
        close(pinfo._fd);
        client_info_free();
        DBG_INFO("error_process %d close \n", pinfo._fd);
    }
    return 0;
}

void client::signalHandler(int sign){
    switch(sign){
    case SIGALRM:
        printf("Caught the SIGALRM signal \n");
        break;
    }
}
};// end of namespace snowlake
