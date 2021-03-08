#include "EasyTcpClient.hpp"
<<<<<<< HEAD
bool g_bRun = true; 

void cmdThread()
=======

void cmdThread(EasyTcpClient* client)
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
{
   while(true)
   {
      char cmdBuf[256] = {};
      scanf("%s",cmdBuf);
      if(0 == strcmp(cmdBuf,"exit"))
      {
<<<<<<< HEAD
         g_bRun = false;
         printf("退出cmdThread线程\n");
         break;
      }
=======
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
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
      else
      {
         printf("不支持命令\n");
      }
   }
      
}
<<<<<<< HEAD
int main()                   
{
   const int cCount = FD_SETSIZE-1; 

   EasyTcpClient *client[cCount];
   
   for(int n = 0;n<cCount;n++)
   {
      client[n] = new EasyTcpClient;
      client[n]->InitSocket();
      //client[n]->Connect("192.168.3.24",4567);
      //client[n]->Connect("127.0.0.1",4567);

   }
   

   //启动线程
   std::thread t1(cmdThread);
   t1.detach();
   Login login;
   int a = 0;
   strcpy(login.userName,"yyy");
   strcpy(login.PassWord,"sks");

   while(g_bRun)
   {

      for( int n = 0;n < cCount;n++)
      {
         //
         
         client[n]->SendData(&login);
         client[n]->OnRun();

      }

   }

   for( int n = 0;n < cCount;n++)
   {
         client[n]->Close();
         delete client[n];
   }

   printf("已退出，任务结束。");
   
   getchar();
   return 0;
=======
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
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
}