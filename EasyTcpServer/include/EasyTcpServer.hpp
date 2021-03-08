#ifndef _EASYTCPSERVER_HPP_
#define _EASYTCPSERVER_HPP_

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
#include <vector>
#include "MessageHeader.hpp"
<<<<<<< HEAD
#define RECV_BUFF_SIZE  10240

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

class EasyTcpServer
{
private:
    /* data */
    SOCKET _sock;
    std::vector<ClientSocket* > g_clients;
public:
    EasyTcpServer(){_sock = INVALID_SOCKET;}
    virtual ~EasyTcpServer(){Close();}

    //初始化socket
    SOCKET InitSocket();
    //绑定IP端口
    int SocketBind(const char* ip,unsigned short port);
    //监听套接字
    SOCKET SocketListen(int n);
    int Accept();
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


=======

class EasyTcpServer
{
private:
    /* data */
    SOCKET _sock;
    std::vector<SOCKET> g_clients;
public:
    EasyTcpServer(){_sock = INVALID_SOCKET;}
    virtual ~EasyTcpServer(){Close();}

    //初始化socket
    SOCKET InitSocket()
    {
#ifdef _WIN32
        //启动Windows socket 2.x 环境
        WORD ver = MAKEWORD(2,2);
        WSADATA dat;
        WSAStartup(ver,&dat);
#endif
    //--用于Socket API 建立简易TCP服务端
        if(INVALID_SOCKET !=  _sock)
        {
            std::cout<<"<socket="<<_sock<<">关闭旧连接..."<<std::endl;
            Close();
        }
        // 建立一个socket 套接字
        _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

        if(INVALID_SOCKET == _sock)
        {
            std::cout<<"错误，建立Socket失败..."<<std::endl;

        }
        else
        {
            std::cout<<"建立Socket=<"<<_sock<<">成功..."<<std::endl;
        }
        return _sock;

    }
    //绑定IP端口
    int SocketBind(const char* ip,unsigned short port)
    {
        // bind 绑定用于接受客户端连接的网络端口
        sockaddr_in _sin = {};
        _sin.sin_family = AF_INET;
        _sin.sin_port = htons(port) ;             // host to net unsigned short

#ifdef _WIN32
        if(ip)
            _sin.sin_addr.S_un.S_addr = inet_addr(ip);
        else
            _sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
        if(ip)
            _sin.sin_addr.s_addr = inet_addr(ip);
        else
            _sin.sin_addr.s_addr = INADDR_ANY; 
#endif
        int ret = bind(_sock,(sockaddr *)&_sin,sizeof(_sin));
        if(SOCKET_ERROR == ret )
        {
            std::cout<<"错误,绑定网络端口失败..."<<std::endl;
            Close();
        }
        else
        {
            std::cout<<"绑定网络端口成功..."<<std::endl;
        }
        return ret;
    }
    //监听套接字
    SOCKET SocketListen(int n)
    {
        if(INVALID_SOCKET == _sock)
            return -1;
        int ret = listen(_sock,n);
            // 3 listen 监听网络端口
        if( ret == SOCKET_ERROR)
            std::cout<<"socket = <"<<_sock<<"> 错误,监听网络端口失败..."<<std::endl;
        else
            std::cout<<"监听网络端口成功..."<<std::endl;

        return ret;  
    }
    int Accept()
    {
        // accept 等待接收客户端
         sockaddr_in clientAddr = {};
         int nAddrLen = sizeof(sockaddr_in);
         SOCKET _cliSock = INVALID_SOCKET;
#ifdef _WIN32
        _cliSock = accept(_sock,(sockaddr*)&clientAddr,&nAddrLen);
#else
        _cliSock = accept(_sock,(sockaddr* )&clientAddr,(socklen_t*)&nAddrLen);
#endif
         if(INVALID_SOCKET == _cliSock)
         {
            std::cout<<"错误，接受到无效客户端SOCKET..."<<std::endl;
         }
         else
         {
            
            NewUserJoin userJoin;
            SendDataAll(&userJoin);
            g_clients.push_back(_cliSock);
            std::cout<<"新客户端加入：socket= "<<_cliSock<<" ,IP:"<<inet_ntoa(clientAddr.sin_addr)<<std::endl;   
         }
        return _cliSock;
    }
    //接收客户端连接
    bool OnRun()
    {
        if(!isRun())
            return false;
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock,&fdRead);
        FD_SET(_sock,&fdWrite);
        FD_SET(_sock,&fdExp);

        SOCKET maxSock = _sock;

        for(int n = (int)g_clients.size()-1; n >= 0;n--)
        {
            FD_SET(g_clients[n],&fdRead);
            if(maxSock < g_clients[n])
                maxSock = g_clients[n];
        }
        //nfds 是一个证书值 是指fd_set集合中所有描述符（socket）的范围，而不是数量
        //既是所有文件描述符最大值+1在windows种这个参数可以写0
        timeval t = {1,0};
        int ret = select(maxSock+1,&fdRead,&fdWrite,&fdExp,&t);          //非阻塞&t
        if(ret < 0)
        {
            std::cout<<"select任务结束。。。"<<std::endl;
            Close();
            return false;
        }
        //判断描述符是否在集合中
        if(FD_ISSET(_sock,&fdRead))
        {
            FD_CLR(_sock,&fdRead);
            Accept();
   
        }
        for(int n = (int)g_clients.size()-1;n >= 0;n--)
        {
            if(FD_ISSET(g_clients[n],&fdRead))
            {
                if(-1 == RecvData(g_clients[n]))
                {
                    auto iter = g_clients.begin() + n;
                    if(iter != g_clients.end())
                    {
                        g_clients.erase(iter);
                    }
                }
            }
        }

        return true;
    }

    bool isRun()
    {
        return _sock != INVALID_SOCKET;
    }
    //关闭socket
    void Close()
    {
        if(_sock == INVALID_SOCKET)
            return ;
#ifdef _WIN32      
        //关闭套接字closesocket
        for(int n = (int)g_clients.size()-1; n >= 0;n--)
        {
            closesocket(g_clients[n]);
        }
        
        closesocket(_sock);
        //-----
        WSACleanup();
        printf("已退出，任务结束。");
#else
        for(int n = (int)g_clients.size - 1;n>=0;n--)
            close(g_clients[n]);
        close(_sock);
        
#endif
        _sock = INVALID_SOCKET;
    }

    //接收数据 处理粘包 拆分包
    int RecvData(SOCKET _cliSock)
    {
        //字符缓冲区
        char szRecv[1024] = {};
        // 5 接收客户端数据 
        int nLen = recv(_cliSock,szRecv,sizeof(DataHeader),0);
        DataHeader* header = (DataHeader* )szRecv;
        if(nLen <= 0)
        {
            std::cout<<"客户端<"<<_cliSock<<">已退出，任务结束"<<std::endl;
            return -1;
        }
        recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);

        OnNetMsg(_cliSock,header);
        return 0;
    }
    virtual void OnNetMsg(SOCKET _cliSock,DataHeader* header)
    {
         // 6 处理请求
        switch(header->cmd)
        {
            case CMD_LOGIN:
            {
                Login* login = (Login* )header;
                std::cout<<"收到命令：CMD_LOGIN 数据长度："<<login->dataLength<<" userName="<<login->userName<<" PassWord="<<login->PassWord<<std::endl;
                //忽略判断用户密码是否正确的过程
                LoginRusult ret;
                SendData(_cliSock,&ret);
            }
            break;
            case CMD_LOGOUT:
            {
               
                Logout* logout = (Logout* )header;
                printf("收到命令：CMD_LOGOUT 数据长度：%d userName=%s \n",logout->dataLength,logout->userName);
                std::cout<<"收到命令：CMD_LOGOUT 数据长度："<<logout->dataLength<<" userName="<<logout->userName<<std::endl;
                //忽略判断用户密码是否正确的过程
                LogoutResult ret;
                SendData(_cliSock,&ret);
            }
            break;
            default:
                DataHeader ret = {0,CMD_ERROR};
                SendData(_cliSock,&ret);
            break;
        }
    }
    //发送指定Socket数据
    int SendData(SOCKET _cSock,DataHeader* header)
    {
        if(isRun()&&header)
        {
            return send(_cSock,(const char*)header,header->dataLength,0);
        }
        return SOCKET_ERROR;
    }
    void SendDataAll(DataHeader* header)
    {
        if(isRun() && header)
        {
            for(int n = (int)g_clients.size()-1; n >= 0;n--)
            {
                
                SendData(g_clients[n],header);
            }
        }
        return ;
    }
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
};

//发送数据
    


#endif