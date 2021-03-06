#include "EasyTcpClient.hpp"

void cmdThread(EasyTcpClient* client)
{
   while(true)
   {
      char cmdBuf[256] = {};
      scanf("%s",cmdBuf);
      if(0 == strcmp(cmdBuf,"exit"))
      {
         client->Close();
         printf("退出cmdThread线程\n");
         break;
      }
      else if(0 == strcmp(cmdBuf,"login"))
      {
         Login login;
         strcpy(login.userName,"jjmj");
         strcpy(login.PassWord,"jjmj");
         client->SendData(&login);
         
      }
      else if(0 == strcmp(cmdBuf,"logout"))
      {
         Logout logout;
         strcpy(logout.userName,"jjmj");
         client->SendData(&logout);
      }
      else
      {
         printf("不支持命令\n");
      }
   }
      
}
int main()
{
 
   EasyTcpClient client;
   client.InitSocket();
   client.Connect("127.0.0.1",4567);

   //启动线程
   std::thread t1(cmdThread,&client);
   t1.detach();
   while(client.isRun())
   {

      client.OnRun();
      //printf("空闲时间处理其他事务。。\n")

   }


    //cout<<"Hello World"<<endl;
    client.Close();
    printf("已退出，任务结束。");
    getchar();
    return 0;
}