#include "EasyTcpClient.hpp"
bool g_bRun = true; 

void cmdThread()
{
   while(true)
   {
      char cmdBuf[256] = {};
      scanf("%s",cmdBuf);
      if(0 == strcmp(cmdBuf,"exit"))
      {
         g_bRun = false;
         printf("退出cmdThread线程\n");
         break;
      }
      else
      {
         printf("不支持命令\n");
      }
   }
      
}
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
}