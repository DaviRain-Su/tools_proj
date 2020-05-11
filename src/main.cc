#include <iostream>
#include <memory>
#include <functional>
#include <string>

using std::string;
using std::bind;
using std::shared_ptr;

#include "../inc/Acceptor.h"
#include "../inc/client.h"
#include "../inc/utils.h"

using std::cout;
using std::endl;
using namespace snowlake;

int main()
{

    const string ip = "127.0.0.1";
    unsigned short port = 88;
    Acceptor acceptor(ip, port);
    shared_ptr<client> ptr_client(new client());
    acceptor.ready();

    int re_register = 0;

    int maxfd;
    maxfd = acceptor.fd();

    DBG_INFO("start ... (maxfd = %d )\n", maxfd);
    fd_set readsets;
    FD_ZERO(&readsets);
    FD_SET(acceptor.fd(), &readsets);
    DBG_INFO("start resgister soft dog. \n");

    FILE *fp;
    char buffer[2014];
    fp = popen("./burn_bit.sh", "r");
    if(nullptr == fp){
        perror("popen");
        return -1;
    }
    fgets(buffer, sizeof(buffer), fp); 
    reg_print("cat_bit complete ... !\n");
    pclose(fp);

    time_t oldtime = 0;
    time(&oldtime);

    fd_set tmpsets;
    struct timeval timeout;

    while(1)
    {
        tmpsets = readsets;
        ptr_client->clients_itor_add_to_set(&tmpsets);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int readys;
        readys = select(maxfd + 1, &tmpsets, NULL, NULL, &timeout);// 接受等待，此处设置为阻塞模式，由setscokopt处理超时
        if (readys < 0){
            DBG_INFO("select error: %d! \n", readys);
            perror("select");
            break;
        }else if ( readys == 0 ){
            continue;
        }else if (readys > 0){
            if( FD_ISSET(acceptor.fd(), &tmpsets) ) { //有链接
                DBG_INFO(" ================== FD is set %d \n", acceptor.fd());
                DBG_INFO(" sockfd = %d \n", acceptor.fd());
                
                if( ptr_client != NULL ){
                    struct sockaddr_in clientaddress;
                    memset(&clientaddress, 0, sizeof(clientaddress));

                    socklen_t addrlen = sizeof(clientaddress);

                    DBG_INFO("start accept (socket_fd = %d) \n", acceptor.fd());

                    int client_fd = acceptor.accept((struct sockaddr*)&clientaddress, &addrlen);
                    DBG_INFO("client_fd (client_fd = %d) \n", client_fd);

                    if(0 <= client_fd){
                        DBG_INFO("get client --- maxfd: %d and client_fd %d \n", maxfd, client_fd);
                        if(maxfd < client_fd){
                            maxfd = client_fd;
                        }

                        FD_SET(client_fd, &tmpsets);
                        
                        ptr_client->client_insert_info(Client_info(client_fd, nullptr, 0, time(nullptr), Frame_head::RX_STATE::RX_STATE_START));
                        
                        timeout.tv_sec = 10;
                        timeout.tv_usec = 0;

                    }else {
                        ptr_client->client_info_free();
                    }
                }
            }

            time_t newtime = 0;
            time(&newtime);

            if( 5 < (newtime - oldtime) ){
                DBG_INFO("newtime: %ld  oldtime: %ld \n", newtime, oldtime);
                oldtime = newtime;
                DBG_INFO(" feed dog in main .... \n");
            }
            ptr_client->clients_itor_read_process(&tmpsets);
        }
    }

    return EXIT_SUCCESS;
}

