#include "EasyTcpServer.hpp"

int main()
{
 
   EasyTcpServer server;
   server.InitSocket();
   server.SocketBind(nullptr,4567);
   server.SocketListen(5);


   
   while(server.isRun())
   {
      server.OnRun();
      
   }

   server.Close();

   std::cout<<"已退出。。。"<<std::endl;
   return 0;
}