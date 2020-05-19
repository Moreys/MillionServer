/*************************************************************************
  @FileName: TcpClient.hpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Fri 15 May 2020 09:41:28 PM PDT
 ************************************************************************/
#include "TcpClient.hpp"

TcpClient::TcpClient()
{
    sock_ = INVALID_SOCKET;
}

TcpClient:: ~TcpClient()
{
    Close();
}

void TcpClient::InitSocket()
{

#ifdef _WIN32
    //启动Windows socket 2.x环境
    WORD ver = MAKEWORD(2, 2);
    WSADATA data;
    WSAStartup(ver, &dat);
#endif

    if(INVALID_SOCKET != sock_)
    {
        printf("[sockset = %d]关闭旧链接...\n", sock_);
        Close();
    }
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == sock_)
    {
        perror("socket");
        printf("error create socks:%d...\n", sock_);
    }
    else
    {
        printf("create socks ok sock_:%d\n", sock_);
    }
}

int TcpClient::Connect(const char * ip, unsigned short port)
{
    if(INVALID_SOCKET == sock_)
    {
        InitSocket();
    }

    sockaddr_in sinAddr_ = {  };
    sinAddr_.sin_family = AF_INET;
    sinAddr_.sin_port = port;
#ifdef _WIN32
    sinAddr_.sin_addr.s_un.s_addr = inet_addr(ip);
#else
    sinAddr_.sin_addr.s_addr = inet_addr(ip);
#endif
    int ret = connect(sock_, (sockaddr*)&sinAddr_, sizeof(sinAddr_));
    if(SOCKET_ERROR == ret)
    {
        perror("connect");
        printf("connect error...\n");
    }
    else
    {
        printf("connect ok...\n");
    }
    return ret;
}

void TcpClient::Close()
{
    if(sock_ != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(sock_);
#else
        close(sock_);
#endif
        sock_ = INVALID_SOCKET;

    }
}

bool TcpClient::OnRun()
{
    if(isRun())
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(sock_, &fdReads);
        timeval time_ = {1, 0};
        int ret = select(sock_ + 1, & fdReads, 0, 0, &time_);
        if(ret < 0)
        {
            printf("[socket:%d]select over one\n", sock_);
            return false;
        }
        if(FD_ISSET(sock_, &fdReads))
        {
            FD_CLR(sock_, &fdReads);
            if(-1 == RecavData(sock_))
            {
                printf("[socket:%d]select over two\n", sock_);
                return false;
            }
        }
        return true;
    }
    return false;
}

int TcpClient::RecavData(SOCKET cSock)
{
    char recvBuff[4096] = {0};

    //每次先读取头部信息，再继续读取数据
    int recvLen = (int)recv(cSock, recvBuff, sizeof(DataHeader), 0);
    DataHeader * pHeader = (DataHeader *)recvBuff;
    if(recvLen <= 0)
    {
        printf("与服务器断开连接 任务结束\n");
        return -1;
    }
    recv(cSock, recvBuff + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader),0);
    OnNetMsg(pHeader);
    return 0;

}

void TcpClient::OnNetMsg(DataHeader * header)
{
    switch(header->cmd)
    {
    case CMD_LOGIN_RESULT:
        {
            LoginResult * pLogin = (LoginResult *)header;
            printf("[OnNetMsg]收到服务器消息，CMD_LOGIN_RESULT，dataLen:%d\n", pLogin->dataLength);
        }
        break;
    case CMD_LOGOUT_RESULT:
        {
            LogoutResult * pLogOut = (LogoutResult*)header;
            printf("[OnNetMsg]收到服务器消息，CMD_LOGOUT_RESULT，dataLen:%d\n", pLogOut->dataLength);
        }
        break;
    case CMD_NEW_USER_JOIN:
        {
            NewUserJoin * pUserJoin = (NewUserJoin*)header;
            printf("[OnNetMsg]收到服务器消息，CMD_NEW_USER_JOIN，dataLen:%d\n", pUserJoin->dataLength);
        }
        break;
    default:
        {
            printf("[OnNetMsg] default:");
        }
        break;
    }
}

int TcpClient::SendData(DataHeader * header)
{
    if(isRun() && header)
    {
        return send(sock_, (const char *)header, header->dataLength, 0);
    }
    return SOCKET_ERROR;
}

