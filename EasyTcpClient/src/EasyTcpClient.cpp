#include "EasyTcpClient.hpp"

int EasyTcpClient::InitSocket()
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
int EasyTcpClient::Connect(const char*ip,unsigned short port)
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
void EasyTcpClient::Close()
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
    //发送数据

    //接收数据
    //处理网络程序
bool EasyTcpClient::OnRun()
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

bool EasyTcpClient::isRun()
{
        return _sock != INVALID_SOCKET;
}

int EasyTcpClient::RecvData(SOCKET _cliSock)
{
        //字符缓冲区
        //char szRecv[RECV_BUFF_SIZE] = {};
        // 5 接收客户端数据 
        int nLen = (int)recv(_cliSock,_szRecv,RECV_BUFF_SIZE,0);
        //std::cout<<"nLen = "<<nLen<<std::endl;
        if(nLen <= 0)
        {
            printf("与服务器断开连接，任务结束\n",_cliSock);
            return -1;
        }
        //将收取到的数据拷贝到消息缓冲区
        memcpy(_szMsgBuf+_lastPos,_szRecv,nLen);
        //消息缓冲区的数据尾部位置后移
        _lastPos += nLen;

        while(_lastPos >= sizeof(DataHeader))
        {
            DataHeader* header = (DataHeader* )_szMsgBuf;
            //判断消息缓冲区的数据长度大于消息长度
            if(_lastPos >= header->dataLength)
            {
                //剩余未处理消息缓冲区数据的长度
                int nSize = _lastPos -header->dataLength;
                //处理网络消息
                OnNetMsg(header);
                //将消息缓冲区剩余未处理的数据前移
                memcpy(_szMsgBuf,_szMsgBuf+header->dataLength,nSize);
                //消息缓冲区的数据尾部位置前移
                _lastPos = nSize;
            }
            else
            {
                //消息缓冲区剩余数据不够一条完整消息
                break;
            }
        }

        return 0;

}

    //响应数据
void EasyTcpClient::OnNetMsg(DataHeader* header)
{
        //  处理请求
        switch(header->cmd)
        {
            case CMD_LOGIN_RESULT:
            {
                
                LoginResult* login = (LoginResult* )header;
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
            case CMD_ERROR:
            {
                std::cout<<"<socket="<<_sock<<">收到服务端消息：CMD_ERROR，数据长度："<<header->dataLength<<std::endl;
            }
            break;            
            default:
            {
                std::cout<<"<socket="<<_sock<<">收到未定义消息，数据长度："<<header->dataLength<<std::endl;
            }
            break;
        }
}

    //发送数据
int EasyTcpClient::SendData(DataHeader* header)
{
        if(isRun() && header)
        {
            return send(_sock,(const char*)header,header->dataLength,0);
        }

        return SOCKET_ERROR;
        
}