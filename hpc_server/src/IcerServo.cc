#include "../inc/IcerServo.h"
#include "../inc/WorkTask.h"
#include "../inc/utils.h"
#include "../inc/md5.h"

#if 0
#include "Message.h"
#include <string.h>

#endif

#include <functional>
using std::bind;
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
using std::ofstream;
using std::stringstream;
using std::vector;
using std::endl;
using std::cout;

namespace  snowlake {

void IcerServo::start()
{
    _threadpool.start();

    _tcpserver.set_connectionCallback(bind(&IcerServo::onConnection, this, std::placeholders::_1));
    _tcpserver.set_MessageCallbacl(bind(&IcerServo::onMessage, this, std::placeholders::_1));
    _tcpserver.set_CloseCallback(bind(&IcerServo::onClose, this, std::placeholders::_1));

    _tcpserver.start();    
}

void IcerServo::onConnection(TcpConnectionPtr conn)
{
    cout << conn->to_string() << " has connected!" << endl;
    conn->send("Welcome to IcerServo!");
}

void IcerServo::onMessage(TcpConnectionPtr conn)
{
    cout << "onMessage ...." << endl;
    vector<char> msg = conn->receive();
    cout << ">> recv msg size is " << msg.size() << endl;
    string tmp_msg(msg.begin(), msg.end());
    string md5 = MD5(tmp_msg).toStr();
    cout << ">> md5 is " << md5 << endl;
    DEBUG(">> md5 is %s\n", md5.data());

    //处理接受到的命令, 由具体的线程去做处理
     WorkTask work(tmp_msg, conn);
     _threadpool.add_task(bind(&WorkTask::process, work));
}

void IcerServo::onClose(TcpConnectionPtr conn)
{
    cout << "onClose ..." << endl;
    cout << conn->to_string() << " has closed! " << endl;
}
}// end of namespace
