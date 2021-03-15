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
//客户端数量
const int cCount = 4000; 
//发送线程
const int tCount = 4;
//客户端数组
EasyTcpClient *client[cCount];

void sendThread(int id)
{
   int c = cCount / tCount;
   int begin = (id - 1)*c;
   int end = id*c;

   for(int n = begin; n < end;n++)
   {
      if(!g_bRun)
         return ;
      
      client[n] = new EasyTcpClient();
   }
   
   for(int n = begin; n < end;n++)
   {
      if(!g_bRun)
         return ;

      client[n]->InitSocket();
      client[n]->Connect("192.168.3.24",4567);
      //client[n]->Connect("127.0.0.1",4567);
      //client[n]->Connect("8.129.238.250",4567);
      std::cout << "count = "<< n <<std::endl;
   }


   Login login;
   strcpy(login.userName,"yyy");
   strcpy(login.PassWord,"sks");

   while(g_bRun)
   {

      for(int n = begin; n < end;n++)
      {
         
         client[n]->SendData(&login);
         //client[n]->OnRun();

      }

   }

   for(int n = begin; n < end;n++)
   {
         client[n]->Close();
         delete client[n];
   }
}
int main()                   
{
   //UI线程
   std::thread t1(cmdThread);
   t1.detach();

   //启动发送线程
   for(int n = 0; n < tCount; n++)
   {
      std::thread t1(sendThread,n+1);
      t1.detach();
   }

   while(g_bRun)
   {
      Sleep(100);
   }
   printf("已退出，任务结束。");
   
   getchar();
   return 0;
}