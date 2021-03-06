#ifndef _EASYTCPSERVER_HPP_
#define _EASYTCPSERVER_HPP_

#ifdef _WIN32
    #define FD_SETSIZE 4096
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <windows.h>
    #pragma comment(lib,"ws2_32.lib")
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>

    #define SOCKET                  int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "CELLTimes.hpp"
#include "MessageHeader.hpp"

#define RECV_BUFF_SIZE  10240
#define _CellServer_THREAD_COUNT    4
extern char _szRecv[RECV_BUFF_SIZE];   //双缓冲区


class ClientSocket
{

private:
    SOCKET _sockfd;     // socket  fd_set    file desc set
    //第二缓冲区 消息缓冲区
    char _szMsgBuf[RECV_BUFF_SIZE*10];
    //消息缓冲区的数据尾部位置
    int _lastPos;
public:
    ClientSocket(SOCKET sockfd = INVALID_SOCKET)
        :_sockfd(sockfd)
    {
        memset(_szMsgBuf,0,sizeof(_szMsgBuf));
        _lastPos = 0;

    }

    virtual ~ClientSocket(){}

    SOCKET sockfd()
    {
        return _sockfd;
    }

    char* msgBuf()
    {
        return _szMsgBuf;
    }
    int getLastPos()
    {
        return _lastPos;
    }
    void setLastPos(int pos)
    {
        _lastPos = pos;
    }
};

class CellServer{

public:
    CellServer(SOCKET sock = INVALID_SOCKET)
    {   
        _sock = sock;
    }
    ~CellServer()
    {
        Close();
        _sock = INVALID_SOCKET;
    }
    bool OnRun();
    bool isRun();
    void Close();
    int RecvData(ClientSocket* pClient);
    virtual void OnNetMsg(SOCKET _cliSock,DataHeader* header);
    void addClients(ClientSocket* pClient);
    void Start();
private:
    SOCKET _sock;
    std::vector<ClientSocket* > g_clients; 
    //缓冲队列  
    std::vector<ClientSocket* > g_clientBuff;
    std::mutex _mutex;
    std::thread* _thread;
};

class EasyTcpServer
{
private:
    /* data */
    SOCKET _sock;
    std::vector<ClientSocket* > g_clients;
    std::vector<CellServer* > _cellServers;
    CELLTimes _tTime;
    int _recvCount;

public:
    EasyTcpServer()
    {
        _sock = INVALID_SOCKET;
        _recvCount = 0;
    }
    virtual ~EasyTcpServer(){Close();}

    //初始化socket
    SOCKET InitSocket();
    //绑定IP端口
    int SocketBind(const char* ip,unsigned short port);
    //监听套接字
    SOCKET SocketListen(int n);
    int Accept();

    void Start();
    //接收客户端连接
    bool OnRun();

    bool isRun();
    //关闭socket
    void Close();
    //发送指定Socket数据
    int SendData(SOCKET _cSock,DataHeader* header);
    void SendDataAll(DataHeader* header);
    int RecvData(ClientSocket* pClient);
    virtual void OnNetMsg(SOCKET _cliSock,DataHeader* header);


};

//发送数据
    


#endif