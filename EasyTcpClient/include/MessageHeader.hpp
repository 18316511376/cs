#ifndef _MESSAGEHEADER_HPP
#define _MESSAGEHEADER_HPP

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
#endif