/*************************************************************************
  @FileName: TcpClient.hpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Fri 15 May 2020 09:41:28 PM PDT
 ************************************************************************/
#ifndef __TCP_CLIENT_HPP__
#define __TCP_CLIENT_HPP__

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <WinSock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>

    #define SOCKET int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR   (-1)
#endif

#include <stdio.h>
#include "MessageHeader.hpp"

class TcpClient
{
public:
    TcpClient();

    virtual ~TcpClient();

    void InitSocket();
    int Connect(const char * ip, unsigned short port);
    void Close();
    bool OnRun();
    int RecavData(SOCKET cSock);
    void OnNetMsg(DataHeader * header);
    int SendData(DataHeader * header);

    bool isRun()
    {
        return sock_ != INVALID_SOCKET;
    }




private:
    SOCKET sock_;
};

#endif //end  of __TCP_CLIENT_HPP__
