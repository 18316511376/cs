#define WIN32_LEAN_AND_MEAN          //2.尽量减少引入会冲突的库     
#include <winsock2.h>               //1.放置Windows.h之前，避免windows重复定义问题
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <vector>
//#pragma comment(lib,"ws2_32.lib")   //添加ws2库
//#include <iostream>
//using namespace std;
enum CMD{
   CMD_LOGIN,
   CMD_LOGIN_RESULT,
   CMD_LOGOUT,
   CMD_LOGOUT_RESULT,
   CMD_NEW_USER_JOIN,
   CMD_ERROR
};
struct DataHeader{
   short dataLength;
   short cmd;
};

struct Login:public DataHeader
{
   Login()
   {
      dataLength = sizeof(Login);
      cmd = CMD_LOGIN;
   }
   char userName[32];
   char PassWord[32];
};
struct LoginRusult:public DataHeader
{
   LoginRusult()
   {
      dataLength = sizeof(LoginRusult);
      cmd = CMD_LOGIN_RESULT;
      result = 0;
   }
   int result;
};
struct Logout:public DataHeader
{
   Logout()
   {
      dataLength = sizeof(Logout);
      cmd = CMD_LOGOUT;
   }
   char userName[32];
};
struct LogoutResult:public DataHeader
{
   LogoutResult()
   {
      dataLength = sizeof(LogoutResult);
      cmd = CMD_LOGOUT_RESULT;  
      result = 0;   
   }
   int result;
};
struct NewUserJoin:public DataHeader
{
   NewUserJoin()
   {
      dataLength = sizeof(NewUserJoin);
      cmd = CMD_NEW_USER_JOIN;  
      sock = 0;   
   }
   int sock;
};
std::vector<SOCKET> g_clients;
int processor(SOCKET _cliSock)
{
     //字符缓冲区
      char szRecv[1024] = {};
      // 5 接收客户端数据 
       int nLen = recv(_cliSock,szRecv,sizeof(DataHeader),0);
      DataHeader* header = (DataHeader* )szRecv;
      if(nLen <= 0)
      {
         printf("客户端<%d>已退出，任务结束\n",_cliSock);
         return -1;
      }
      if(nLen >= sizeof(DataHeader))

 
         // 6 处理请求
     switch(header->cmd)
      {
         case CMD_LOGIN:
         {
               
            recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);
            Login* login = (Login* )szRecv;
            printf("收到命令：CMD_LOGIN 数据长度：%d userName=%s PassWord=%s \n",login->dataLength,login->userName,login->PassWord);
            //忽略判断用户密码是否正确的过程
            LoginRusult ret;
            send(_cliSock,(char*)&ret,sizeof(LoginRusult),0);
         }
            break;
         case CMD_LOGOUT:
         {
               
            recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);
            Logout* logout = (Logout* )szRecv;
            printf("收到命令：CMD_LOGOUT 数据长度：%d userName=%s \n",logout->dataLength,logout->userName);
            //忽略判断用户密码是否正确的过程
            LogoutResult ret;
            send(_cliSock,(char*)&ret,sizeof(ret),0);
         }
            break;
         default:
            DataHeader header = {0,CMD_ERROR};
            send(_cliSock,(char*)&header,sizeof(header),0);
            break;
      }
      return 0;
}

int main()
{
    WORD ver = MAKEWORD(2,2);
    WSADATA dat;
    WSAStartup(ver,&dat);

    //-----
    //--用于Socket API 建立简易TCP服务端

    // 1 建立一个socket 套接字
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    // 2 bind 绑定用于接受客户端连接的网络端口
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567) ;             // host to net unsigned short
    _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if( SOCKET_ERROR ==  bind(_sock,(sockaddr *)&_sin,sizeof(_sin)))
    {
      printf("错误,绑定网络端口失败。。。\n");
    }
    else
    {
      printf("绑定网络端口成功。。。\n");
    }
   
    // 3 listen 监听网络端口
    if( SOCKET_ERROR ==  listen(_sock,5))
    {
       printf("错误,监听网络端口失败。。。\n");
    }
    else
    {
       printf("监听网络端口成功。。。\n");

    }    
   
   while(true)
   {
      fd_set fdRead;
      fd_set fdWrite;
      fd_set fdExp;

      FD_ZERO(&fdRead);
      FD_ZERO(&fdWrite);
      FD_ZERO(&fdExp);

      FD_SET(_sock,&fdRead);
      FD_SET(_sock,&fdWrite);
      FD_SET(_sock,&fdExp);

      for(int n = (int)g_clients.size()-1; n >= 0;n--)
      {
         FD_SET(g_clients[n],&fdRead);
      }
      //nfds 是一个证书值 是指fd_set集合中所有描述符（socket）的范围，而不是数量
      //既是所有文件描述符最大值+1在windows种这个参数可以写0
      timeval t = {1,0};
      int ret = select(_sock+1,&fdRead,&fdWrite,&fdExp,&t);          //非阻塞&t
      if(ret < 0)
      {
         printf("select任务结束。\n");
         break;
      }
      if(FD_ISSET(_sock,&fdRead))
      {
         FD_CLR(_sock,&fdRead);
         // 4 accept 等待接收客户端
         sockaddr_in clientAddr = {};
         int nAddrLen = sizeof(sockaddr_in);
         SOCKET _cliSock = INVALID_SOCKET;

         _cliSock = accept(_sock,(sockaddr* )&clientAddr,&nAddrLen);
         if(INVALID_SOCKET == _cliSock)
         {
            printf("错误，接受到无效客户端SOCKET。。。\n");
         }
         for(int n = (int)g_clients.size()-1; n >= 0;n--)
         {
            NewUserJoin userJoin;
            send(g_clients[n],(const char* )&userJoin,sizeof(NewUserJoin),0);
         }
         g_clients.push_back(_cliSock);
         printf("新客户端加入：socket= %d ,IP ：%s \n",_cliSock,inet_ntoa(clientAddr.sin_addr));
         
      }
      for(size_t n = 0; n < fdRead.fd_count;n++)
      {
         if(-1 == processor(fdRead.fd_array[n]))
         {
            auto iter = find(g_clients.begin(),g_clients.end(),fdRead.fd_array[n]);
            if(iter != g_clients.end())
            {
               g_clients.erase(iter);
            }
         }
      }
      
      //printf("空闲时间处理其他事务。。\n");
   }

   for(int n = (int)g_clients.size()-1; n >= 0;n--)
   {
      closesocket(g_clients[n]);
   }

    // 8 关闭套接字closesocket
    closesocket(_sock);
    //-----
    WSACleanup();
    printf("已退出，任务结束。");
    getchar();
    //cout<<"Hello World"<<endl;
    return 0;
}