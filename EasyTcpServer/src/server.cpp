#include "EasyTcpServer.hpp"
#include <thread>

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
 
   EasyTcpServer server;
   server.InitSocket();
   server.SocketBind(nullptr,4567);
   server.SocketListen(5);

   std::thread t1(cmdThread);
   t1.detach();

   
   while(g_bRun)
   {
      server.OnRun();
      
   }

   server.Close();

   std::cout<<"已退出。。。"<<std::endl;
   return 0;
}