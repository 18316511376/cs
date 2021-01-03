#define WIN32_LEAN_AND_MEAN          //2.尽量减少引入会冲突的库     
#include <winsock2.h>               //1.放置Windows.h之前，避免windows重复定义问题
#include <windows.h>
#include <stdio.h>
enum CMD{
   CMD_LOGIN,
   CMD_LOGIN_RESULT,
   CMD_LOGOUT,
   CMD_LOGOUT_RESULT,
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
        // 3 输入命令
        char cmdBuf[128] = {};
        scanf("%s",cmdBuf);
        // 4 处理请求命令
        if (0 == strcmp(cmdBuf,"exit"))
        {
            printf("收到exit命令，任务结束。");
            break;
        }
        else if(0 == strcmp(cmdBuf,"login"))
        {
            Login login;
            strcpy(login.userName,"jjmj");
            strcpy(login.PassWord,"123456");
            // 5 向服务器发送命令 
            send(_sock,(const char* )&login,sizeof(login),0);
            //接收服务器返回的数据
            LoginRusult loginRet;
            recv(_sock,(char* )&loginRet,sizeof(loginRet),0);
            printf("LoginResult:%d\n",loginRet.result);


        }
        else if(0 == strcmp(cmdBuf,"logout"))
        {
            // 5 向服务器发送命令 
            Logout logout;
            strcpy(logout.userName,"jjmj");
            send(_sock,(const char *)&logout,sizeof(logout),0);
            //接收服务器返回的数据
            LogoutResult logoutRet;
            recv(_sock,(char* )&logoutRet,sizeof(logoutRet),0);
            printf("LogoutResult:%d\n",logoutRet.result);
        }
        else
        {
            printf("不支持的命令，请重新输入。\n");
        }
        

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