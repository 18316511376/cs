<<<<<<< HEAD
#ifndef _MESSAGEHEADER_HPP_
#define _MESSAGEHEADER_HPP_
=======
#ifndef _MESSAGEHEADER_HPP
#define _MESSAGEHEADER_HPP
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a

enum CMD{
   CMD_LOGIN,
   CMD_LOGIN_RESULT,
   CMD_LOGOUT,
   CMD_LOGOUT_RESULT,
   CMD_NEW_USER_JOIN,
   CMD_ERROR
};
struct DataHeader{
<<<<<<< HEAD
   DataHeader()
   {
      dataLength = sizeof(DataHeader);
      cmd = CMD_ERROR;
   }
=======
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
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
<<<<<<< HEAD
   char data[932];
};
struct LoginResult:public DataHeader
{
   LoginResult()
   {
      dataLength = sizeof(LoginResult);
=======
};
struct LoginRusult:public DataHeader
{
   LoginRusult()
   {
      dataLength = sizeof(LoginRusult);
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
      cmd = CMD_LOGIN_RESULT;
      result = 0;
   }
   int result;
<<<<<<< HEAD
   char data[992];
=======
>>>>>>> 18facd1fd6e1be9798ad6108f2e8cfc04c7f9f3a
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