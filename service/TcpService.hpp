/*************************************************************************
  @FileName: TcpService.hpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Sat 16 May 2020 02:26:50 AM PDT
 ************************************************************************/
#ifndef __TCP_SERVICE_HPP__
#define __TCP_SERVICE_HPP__

#ifdef _WIN32

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
const short Socket_Error = -1;
#endif

#include <stdio.h>
#include <vector>
#include "MessageHeader.hpp"



class TcpService
{
public:
    TcpService();
    virtual ~TcpService();

    int InitSocket();
    int Bind(const char * ip, unsigned short port);
    int Listen(int num);
    int Accept();
    void Close();
    bool OnRun();
    bool isRun();
    int RecvData(int csock);
    void OnNetMsg(int csock, DataHeader * pHeader);
    int SendData(int csock, DataHeader * pHeader);
    void SendDataToAll(DataHeader * pHeader);

private:
    int sockFd_;
    std::vector<int> g_clients;
};

#endif//end of __TCP_SERVICE_HPP__
