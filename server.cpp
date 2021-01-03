#define WIN32_LEAN_AND_MEAN          //2.尽量减少引入会冲突的库     
#include <winsock2.h>               //1.放置Windows.h之前，避免windows重复定义问题
#include <windows.h>
#include <stdio.h>
#include <string.h>
//#pragma comment(lib,"ws2_32.lib")   //添加ws2库
//#include <iostream>
//using namespace std;
enum CMD{
   CMD_LOGIN,
   CMD_LOGOUT,
   CMD_ERROR
};
struct DataHeader{
   short dataLength;
   short cmd;
};

struct Login{
   char userName[32];
   char PassWord[32];
};
struct LoginRusult{
   int result;
};
struct Logout{
   char userName[32];
};
struct LogoutResult{
   int result;
};
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
    // 4 accept 等待接收客户端
    sockaddr_in clientAddr = {};
    int nAddrLen = sizeof(sockaddr_in);
    SOCKET _cliSock = INVALID_SOCKET;

   _cliSock = accept(_sock,(sockaddr* )&clientAddr,&nAddrLen);
   if(INVALID_SOCKET == _cliSock)
   {
      printf("错误，接受到无效客户端SOCKET。。。\n");
   }
   printf("新客户端加入：IP ：%s \n",inet_ntoa(clientAddr.sin_addr));
   
   char _recvBuf[128] = {};
    while(true)
    {
         DataHeader header = {};
         // 5 接收客户端数据 
         int nLen = recv(_cliSock,(char*)&header,sizeof(header),0);
         if(nLen <= 0)
         {
           printf("客户端已退出，任务结束\n");
           break;
         }
         printf("收到命令：%d 数据长度：%d\n",header.cmd,header.dataLength);
         switch(header.cmd)
         {
            case CMD_LOGIN:
            {
               Login login = {};
               recv(_cliSock,(char*)&login,sizeof(Login),0);
               //忽略判断用户密码是否正确的过程
               LoginRusult ret = {1};
               send(_cliSock,(char*)&header,sizeof(DataHeader),0);
               send(_cliSock,(char*)&ret,sizeof(LoginRusult),0);
            }
               break;
            case CMD_LOGOUT:
            {
               Logout logout = {};
               recv(_cliSock,(char*)&logout,sizeof(logout),0);
               //忽略判断用户密码是否正确的过程
               LogoutResult ret = {1};
               send(_cliSock,(char*)&header,sizeof(header),0);
               send(_cliSock,(char*)&ret,sizeof(ret),0);
            }
               break;
            default:
               header.cmd = CMD_ERROR;
               header.dataLength = 0;
               send(_cliSock,(char*)&header,sizeof(header),0);
               break;
         }
         // 6 处理请求
    }


    // 6 关闭套接字closesocket
    closesocket(_sock);
    //-----
    WSACleanup();
    printf("已退出，任务结束。");
    getchar();
    //cout<<"Hello World"<<endl;
    return 0;
}