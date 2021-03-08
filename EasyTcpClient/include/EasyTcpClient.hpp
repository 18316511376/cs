#ifndef _EASYTCPCLIENT_HPP_
#define _EASYTCPCLIENT_HPP_

#ifdef _WIN32
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

<<<<<<< HEAD
#define RECV_BUFF_SIZE  10240



=======
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
class EasyTcpClient
{
private:
    SOCKET _sock;
public:
    EasyTcpClient()
<<<<<<< HEAD
    { 
    
    }
=======
    {}
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
    virtual ~EasyTcpClient()
    {
        Close();
    }

    //初始化socket
<<<<<<< HEAD
    int InitSocket();

    //连接服务器
    int Connect(const char*ip,unsigned short port);

    //关闭套接字socket
    void Close();
=======
    int InitSocket()
    {
#ifdef _WIN32
        //启动Win Sock  2.x 环境
        WORD ver = MAKEWORD(2,2);
        WSADATA dat;
        WSAStartup(ver,&dat);

#endif
        if(INVALID_SOCKET == _sock)
        {
            printf("<socket = %d>关闭旧连接...\n",_sock);
            Close();
        }
    //1 建立一个socket
        _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (INVALID_SOCKET == _sock)
        {
            printf("错误，建立Socket失败...\n");
            return 0;
        }
        else
        {
            printf("成功，建立Socket成功...\n");
        }
        
        return 1;
    }

    //连接服务器
    int Connect(const char*ip,unsigned short port)
    {
        if(INVALID_SOCKET == _sock)
        {
            InitSocket();
        }
        // 2连接服务端 connect
        sockaddr_in _sin = {};
        _sin.sin_family = AF_INET;
        _sin.sin_port = htons(port) ;             // host to net unsigned short
#ifdef _WIN32

        _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
        _sin.sin_addr.s_addr = inet_addr(ip);
#endif

        int ret = connect(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
        if(INVALID_SOCKET == ret)
        {
            printf("错误，连接服务器失败...\n");
        }
        else
        {
            printf("成功，连接服务器成功...\n");
        }

        return ret;
    }

    //关闭套接字socket
    void Close()
    {
        if(_sock == INVALID_SOCKET)
        {
            return ;
        }

         // 4 关闭套接字closesocket
#ifdef _WIN32
        closesocket(_sock);
        //清楚Windows socket环境
        WSACleanup();
#else
        close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
    //发送数据

    //接收数据
    //处理网络程序
<<<<<<< HEAD
    bool OnRun();

    bool isRun();

//接收缓冲区
    char _szRecv[RECV_BUFF_SIZE] = {};   //双缓冲区
    //第二缓冲区 消息缓冲区
    char _szMsgBuf[RECV_BUFF_SIZE*10] = {};
    int _lastPos = 0;

    //网络接收
    int RecvData(SOCKET _cliSock);

    //响应数据
    virtual void OnNetMsg(DataHeader* header);

    //发送数据
    int SendData(DataHeader* header);
=======
    bool OnRun()
    {
        if(isRun())
        {
            fd_set fdReads;
            FD_ZERO(&fdReads);
            //将描述符添加到集合中
            FD_SET(_sock,&fdReads);
            timeval t = {1,0};
            int ret = select(_sock,&fdReads,0,0,&t);
            if(ret < 0)
            {
                printf("<socket = %d>select任务结束1\n",_sock);
                return false;
            }

            //判断描述符（socket）是否在集合
            if(FD_ISSET(_sock,&fdReads))
            {
                FD_CLR(_sock,&fdReads);
                if(-1 == RecvData(_sock))
                {
                    printf("<socket = %d>select任务结束2\n",_sock);
                    return false;
                }
            } 
            return true;
        }

        return false;
    }

    bool isRun()
    {
        return _sock != INVALID_SOCKET;
    }

    //网络接收
    int RecvData(SOCKET _cliSock)
    {
        //字符缓冲区
        char szRecv[4096] = {};
        // 5 接收客户端数据 
        int nLen = recv(_cliSock,szRecv,sizeof(DataHeader),0);
        DataHeader* header = (DataHeader* )szRecv;
        if(nLen <= 0)
        {
            printf("与服务器断开连接，任务结束\n",_cliSock);
            return -1;
        }
        
        recv(_cliSock,szRecv + sizeof(DataHeader),header->dataLength - sizeof(DataHeader),0);
 
        OnNetMsg(header);
        return 0;
        

    }

    //响应数据
    virtual void OnNetMsg(DataHeader* header)
    {
        //  处理请求
        switch(header->cmd)
        {
            case CMD_LOGIN_RESULT:
            {
                
                LoginRusult* login = (LoginRusult* )header;
                std::cout<<"收到服务器消息：CMD_LOGIN_RESULT 数据长度："<<login->dataLength<<std::endl;               

            }
            break;
            case CMD_LOGOUT_RESULT:
            {
                LogoutResult* logout = (LogoutResult* )header;
                std::cout<<"收到服务器消息：CMD_LOGOUT_RESULT 数据长度："<<logout->dataLength<<std::endl;                  

            }
                break;
            case CMD_NEW_USER_JOIN:
            {
                NewUserJoin* userJoin = (NewUserJoin* )header;
                std::cout<<"收到服务器消息：CMD_NEW_USER_JOIN 数据长度："<<userJoin->dataLength<<std::endl;             
            }
            break;
            default:
                DataHeader header = {0,CMD_ERROR};
                SendData(&header);
            break;
        }
    }

    //发送数据
    int SendData(DataHeader* header)
    {
        if(isRun() && header)
        {
            return send(_sock,(const char*)header,header->dataLength,0);
        }

        return SOCKET_ERROR;
        
    }
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
};
#endif