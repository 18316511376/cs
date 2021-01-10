#define WIN32_LEAN_AND_MEAN          //2.尽量减少引入会冲突的库     
#include <winsock2.h>               //1.放置Windows.h之前，避免windows重复定义问题
#include <windows.h>
#include <stdio.h>
#include <iostream>
//#include <stdlib.h>
//#include <pthread.h>
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

int processor(SOCKET _cliSock)
{
     //字符缓冲区
      char szRecv[10240] = {};
      // 5 接收客户端数据 
       int nLen = recv(_cliSock,szRecv,sizeof(DataHeader),0);
      DataHeader* header = (DataHeader* )szRecv;
      if(nLen <= 0)
      {
         printf("与服务器断开连接，任务结束\n",_cliSock);
         return -1;
      }
      if(nLen >= sizeof(DataHeader))

 
         // 6 处理请求
     switch(header->cmd)
      {
         case CMD_LOGIN_RESULT:
         {
            recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);
            LoginRusult* login = (LoginRusult* )szRecv;
            printf("收到服务器消息：CMD_LOGIN_RESULT 数据长度：%d \n",login->dataLength);               

         }
            break;
         case CMD_LOGOUT_RESULT:
         {
            recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);
            LogoutResult* logout = (LogoutResult* )szRecv;
            printf("收到服务器消息：CMD_LOGIN_RESULT 数据长度：%d \n",logout->dataLength);                  

         }
            break;
         case CMD_NEW_USER_JOIN:
         {
            recv(_cliSock,szRecv+sizeof(DataHeader),header->dataLength-sizeof(DataHeader),0);
            NewUserJoin* userJoin = (NewUserJoin* )szRecv;
            printf("收到服务器消息：CMD_LOGIN_RESULT 数据长度：%d \n",userJoin->dataLength);             
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
    //--用于Socket API 建立简易TCP客户端
    // 1 建立一个socket 套接字
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,0);
    if (INVALID_SOCKET == _sock)
    {
        printf("错误，建立Socket失败...\n");
    }
    else
    {
        printf("成功，建立Socket成功...\n");
    }
    // 2连接服务端 connect
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567) ;             // host to net unsigned short
    _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    int ret = connect(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
    if(INVALID_SOCKET == ret)
    {
        printf("错误，建立Socket失败...\n");
    }
    else
    {
        printf("成功，建立Socket成功...\n");
    }
    while(true)
    {
      fd_set fdReads;
      FD_ZERO(&fdReads);
      //将描述符添加到集合中
      FD_SET(_sock,&fdReads);
      timeval t = {1,0};
      int ret = select(_sock,&fdReads,0,0,&t);
      if(ret < 0)
      {
         printf("select任务结束1\n");
         break;
      }
      //判断描述符（socket）是否在集合
      if(FD_ISSET(_sock,&fdReads))
      {
         FD_CLR(_sock,&fdReads);
         if(-1 == processor(_sock))
         {
            printf("select任务结束2");
            break;
         }
      }
      
      printf("空闲时间处理其他事务。。\n");
      Login login;
      strcpy(login.userName,"jjmj");
      strcpy(login.PassWord,"jjmj");
      send(_sock,(const char* )&login,sizeof(login),0);
      Sleep(1000);

   }

    // 4 关闭套接字closesocket
    closesocket(_sock);
    //-----
    WSACleanup();
    //cout<<"Hello World"<<endl;
    printf("已退出，任务结束。");
    getchar();
    return 0;
}