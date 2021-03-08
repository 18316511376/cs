#include "EasyTcpServer.hpp"

 char _szRecv[RECV_BUFF_SIZE] = {};
    //初始化socket
SOCKET EasyTcpServer::InitSocket()
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
int EasyTcpServer::SocketBind(const char* ip,unsigned short port)
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
SOCKET EasyTcpServer::SocketListen(int n)
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

int EasyTcpServer::Accept()
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
            g_clients.push_back(new ClientSocket(_cliSock));
            std::cout<<"新客户端加入：socket= "<<_cliSock<<" ,IP:"<<inet_ntoa(clientAddr.sin_addr)<<std::endl;   
         }
        return _cliSock;
}
    //接收客户端连接
    
bool EasyTcpServer::OnRun()
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
            FD_SET(g_clients[n]->sockfd(),&fdRead);
            if(maxSock < g_clients[n]->sockfd())
                maxSock = g_clients[n]->sockfd();
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
            if(FD_ISSET(g_clients[n]->sockfd(),&fdRead))
            {
                if(-1 == RecvData(g_clients[n]))
                {
                    auto iter = g_clients.begin() + n;
                    if(iter != g_clients.end())
                    {
                        delete g_clients[n];
                        g_clients.erase(iter);
                    }
                }
            }
        }

        return true;
}

bool EasyTcpServer::isRun()
{
        return _sock != INVALID_SOCKET;
}
    //关闭socket
void EasyTcpServer::Close()
{
        if(_sock == INVALID_SOCKET)
            return ;
#ifdef _WIN32      
        //关闭套接字closesocket
        for(int n = (int)g_clients.size()-1; n >= 0;n--)
        {
            closesocket(g_clients[n]->sockfd());
            delete g_clients[n];
        }
        
        closesocket(_sock);
        //-----
        WSACleanup();
        printf("已退出，任务结束。");
#else
        for(int n = (int)g_clients.size - 1;n>=0;n--)
        {
            close(g_clients[n]->sockfd());
            delete g_clients[n];
        }
        close(_sock);
#endif
        g_clients.clear();
        _sock = INVALID_SOCKET;
}

int EasyTcpServer::RecvData(ClientSocket* pClient)
{

        //  接收客户端数据 
        int nLen = (int)recv(pClient->sockfd(),_szRecv,RECV_BUFF_SIZE,0);
       // std::cout<<"nLen = "<<nLen<<std::endl;
        if(nLen <= 0)
        {
            std::cout<<"客户端<"<<pClient->sockfd()<<">已退出，任务结束"<<std::endl;
            return -1;
        }

        //将收取的数据拷贝到消息缓冲区
        memcpy(pClient->msgBuf()+pClient->getLastPos(),_szRecv,nLen);
        //消息缓冲区的数据尾部位置后移
        pClient->setLastPos(pClient->getLastPos()+ nLen);
        //判断后移的大小是否大于数据头
        while(pClient->getLastPos() >= sizeof(DataHeader))
        {
            DataHeader* header = (DataHeader*)pClient->msgBuf();
            //判断消息缓冲区的数据长度大于消息长度
            if(pClient->getLastPos() >= header->dataLength)
            {

                //剩余未处理消息缓冲区数据的长度
                int nSize = pClient->getLastPos()-header->dataLength;
                //处理数据
                OnNetMsg(pClient->sockfd(),header);

                //将消息缓冲区剩余未处理的数据前移
                memcpy(pClient->msgBuf(),pClient->msgBuf()+header->dataLength,nSize);
                //消息缓冲的数据尾部位置前移
                pClient->setLastPos(nSize);
            }
            else
            {
                //消息缓冲区剩余数据不够一条完整消息
                break;
            }
        } 
        return 0;
}

void EasyTcpServer::OnNetMsg(SOCKET _cliSock,DataHeader* header)
{
         // 6 处理请求
        switch(header->cmd)
        {
            case CMD_LOGIN:
            {
                Login* login = (Login* )header;
                std::cout<<"收到命令：CMD_LOGIN 数据长度："<<login->dataLength<<" userName="<<login->userName<<" PassWord="<<login->PassWord<<std::endl;
                //忽略判断用户密码是否正确的过程
                LoginResult ret;
                SendData(_cliSock,&ret);
            }
            break;
            case CMD_LOGOUT:
            {
               
                Logout* logout = (Logout* )header;
                std::cout<<"收到命令：CMD_LOGOUT 数据长度："<<logout->dataLength<<" userName="<<logout->userName<<std::endl;
                //忽略判断用户密码是否正确的过程
                LogoutResult ret;
                SendData(_cliSock,&ret);
            }
            break;
            default:
            {
                std::cout<<"<socket = "<<_sock<<"> 收到未定义消息，数据长度："<<header->dataLength<<" userName="<<std::endl;
            }
                //DataHeader ret = {0,CMD_ERROR};
                //SendData(_cliSock,&ret);
            break;
        }
}
    //发送指定Socket数据
int EasyTcpServer::SendData(SOCKET _cSock,DataHeader* header)
{
        if(isRun()&&header)
        {
            return send(_cSock,(const char*)header,header->dataLength,0);
        }
        return SOCKET_ERROR;
}
    
void EasyTcpServer::SendDataAll(DataHeader* header)
{
        if(isRun() && header)
        {
            for(int n = (int)g_clients.size()-1; n >= 0;n--)
            {
                
                SendData(g_clients[n]->sockfd(),header);
            }
        }
        return ;
}