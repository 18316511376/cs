#ifndef _EASYTCPCLIENT_HPP_
#define _EASYTCPCLIENT_HPP_

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
#include <thread>
#include "MessageHeader.hpp"

#define RECV_BUFF_SIZE  10240



class EasyTcpClient
{
private:
    SOCKET _sock;
    //接收缓冲区
    char _szRecv[RECV_BUFF_SIZE];   //双缓冲区
    //第二缓冲区 消息缓冲区
    char _szMsgBuf[RECV_BUFF_SIZE*10];
    int _lastPos = 0;
public:
    EasyTcpClient()
    { 
        memset(_szMsgBuf,0,sizeof(_szMsgBuf));
        _lastPos = 0;
    }
    virtual ~EasyTcpClient()
    {
        Close();
    }

    //初始化socket
    int InitSocket();

    //连接服务器
    int Connect(const char*ip,unsigned short port);

    //关闭套接字socket
    void Close();
    //发送数据

    //接收数据
    //处理网络程序
    bool OnRun();

    bool isRun();



    //网络接收
    int RecvData(SOCKET _cliSock);

    //响应数据
    virtual void OnNetMsg(DataHeader* header);

    //发送数据
    int SendData(DataHeader* header);
};
#endif