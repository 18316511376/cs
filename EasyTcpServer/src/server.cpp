#include "EasyTcpServer.hpp"
<<<<<<< HEAD
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
=======

>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
int main()
{
 
   EasyTcpServer server;
   server.InitSocket();
   server.SocketBind(nullptr,4567);
   server.SocketListen(5);

<<<<<<< HEAD
   std::thread t1(cmdThread);
   t1.detach();

   
   while(g_bRun)
=======

   
   while(server.isRun())
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
   {
      server.OnRun();
      
   }

   server.Close();

   std::cout<<"已退出。。。"<<std::endl;
   return 0;
}