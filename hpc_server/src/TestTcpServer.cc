#include "../inc/Threadpool.h"
#include "../inc/TcpServer.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
using std::stringstream;
using std::cout;
using std::endl;
using std::string;

using namespace snowlake;
Threadpool * gThreadpool = nullptr;

class TaskUnit {
public:
    TaskUnit(const string & msg, const TcpConnectionPtr &conn)
        : _msg(msg)
        , _conn(conn)
    {}
    //运行在线程池中的某一个子线程中
    void process(){
        //decode
        //compute
        //encode
#if 0
    cout << "打印接受到的命令 " << _msg << endl;
    //接受文件大小 
    vector<char> recv_file_size = _conn->receive();
    cout << "recv_file_size is " << recv_file_size.size() << endl;
    for(auto item : recv_file_size) {
        cout << item << "";
    }
    cout << endl;
    string iter_str(recv_file_size.begin(), recv_file_size.end());
    cout << "iter is " << iter_str << endl;
    cout << "iter_str size is " << iter_str.size() << endl;
    //打印文件大小
    stringstream oss(iter_str);
    int file_size = 0;
    oss >> file_size;
    cout << ">> recv file size is " << file_size << endl;    
#else
        string response = _msg;// 要返回给客户端的消息
        //_conn->send(response);//由线程池的线程， 计算线程完成数据的发送，在
        //在设计上来说，是不合理的
        //数据的发送工作要交还给IO线程reactor所在的线程完成
        //将send的函数执行延迟道io线程去操作
        cout << "response size is " << response.size() << endl;
        _conn->send_in_loop( response );
#endif
    }
private:
    string _msg;
    TcpConnectionPtr _conn;
};

void onConnection(const TcpConnectionPtr &conn)
{
    cout << conn->to_string() << " has connected!" << endl;
    conn->send("welcome to server.");
}

void onMessage(const TcpConnectionPtr & conn){
    cout << "onMessage ..." << endl;
    vector<char>  msg = conn->receive();
    //cout << ">> receive msg from client: " << msg << endl;
    //业务的处理要交给贤臣池处理
    //decode
    //compute
    //encode
    //::sleep(2);
    string m(msg.begin(), msg.end());
    cout << "content is  " << m << endl; 
    cout << "m size is " << m.size() << endl;
    TaskUnit t1(m, conn);

    gThreadpool->add_task(std::bind(&TaskUnit::process, t1));
}

void onClose(const TcpConnectionPtr & conn){
    cout << "onClose ..." << endl;
    cout << conn->to_string() << " has closed！" << endl;
}


int main(){
    Threadpool threadpool(4, 10);
    threadpool.start();

    gThreadpool = &threadpool;

    TcpServer server("192.168.3.199", 8888);
    server.set_connectionCallback(onConnection);
    server.set_MessageCallbacl(onMessage);
    server.set_CloseCallback(onClose);

    server.start();

    return 0;
}
