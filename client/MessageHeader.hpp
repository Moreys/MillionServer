/*************************************************************************
  @FileName: MessageHeader.hpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Sat 14 Mar 2020 11:29:19 PM CST
 ************************************************************************/
#ifndef __MESSAGE_HEADER_HPP__
#define __MESSAGE_HEADER_HPP__


enum CMD//这里定义了后面后可以使用
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
};

struct DataHeader
{

    //DataHeader()
    //{
    //    dataLength = sizeof(DataHeader);
    //    cmd = CMD_ERROR; //默认值
    //}
    short dataLength; //数据的长度
   short cmd; //每次需要执行的任务
};

struct Login : public DataHeader
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }

    char userName[32];
    char passWord[32];
    char data[32];
};

struct LoginResult : public DataHeader
{

    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
    char data[92];
};

struct Logout : public DataHeader
{
    Logout()
    {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};

struct LogoutResult : public DataHeader
{
    LogoutResult()
    {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;
    }
    int result;
};

struct NewUserJoin : public DataHeader
{
    NewUserJoin()
    {
        dataLength = sizeof(NewUserJoin);
        cmd = CMD_NEW_USER_JOIN;
        sock = 0;
    }
    int sock;
};

#endif //end of __MESSAGE_HEADER_HPP__

