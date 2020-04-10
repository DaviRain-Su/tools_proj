#include <iostream>


#include "Acceptor.h"
#include "client.h"
#include "utils.h"

using std::cout;
using std::endl;
using namespace snowlake;

int main()
{

    Acceptor acceptor(1000);
    client client_;
    acceptor.ready();
    
    int re_register = 0;

    int maxfd;
    maxfd = acceptor.fd();
    
    DBG_INFO("start ... (maxfd = %d )\n", maxfd);
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(acceptor.fd(), &readset);
    DBG_INFO("start resgister soft dog. \n");
    
    


    cout << "Hello world" <<endl;
    return 0;
}

