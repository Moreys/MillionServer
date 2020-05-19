/*************************************************************************
  @FileName: TcpService.hpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Sat 16 May 2020 02:26:50 AM PDT
 ************************************************************************/
#include "TcpService.hpp"
TcpService::TcpService()
{
    sockFd_ = INVALID_SOCKET;
}
TcpService::~TcpService()
{
    Close();
}

int TcpService::InitSocket()
{
#ifdef _WIN32

#endif
    if(INVALID_SOCKET != sockFd_)
    {
        printf("[socket:%d]关闭旧链接..\n",(int)sockFd_);
        Close();
    }
    sockFd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == sockFd_)
    {
        perror("socket");
        printf("create sockfd error\n");
    }
    else
    {
        printf("create sockfd ok\n");
    }
    return sockFd_;
}

int TcpService::Bind(const char * ip, unsigned short port)
{
    sockaddr_in sinAddr = { };
    sinAddr.sin_family = AF_INET;
    sinAddr.sin_port = port;
#ifdef _WIN32

#else
    if(ip)
    {
        sinAddr.sin_addr.s_addr = inet_addr(ip);
    }
    else 
    {
        sinAddr.sin_addr.s_addr = INADDR_ANY;
    }
#endif

    int ret = bind(sockFd_, (sockaddr*)&sinAddr, sizeof(sockaddr_in));
    if(Socket_Error == ret)
    {
        perror("bind");
        printf("bind error port:%d\n", port);
    }
    else
    {
        printf("bind ok port:%d \n", port);
    }
    return ret;
}

int TcpService::Listen(int num)
{
    int ret = listen(sockFd_, num);
    if(Socket_Error == ret)
    {
        perror("listen");
        printf("Listen error socket:%d\n", sockFd_);
    }
    else
    {
        printf("Listen socket:%d ok\n", sockFd_);
    }
    return  ret;
}

int TcpService::Accept()
{
    sockaddr_in cliAddr = {};
    int nAddrLen = sizeof(sockaddr_in);
    int cSock_ = INVALID_SOCKET;

#ifdef _WIN32
#else
    cSock_ = accept(sockFd_, (sockaddr*)&cliAddr, (socklen_t *)&nAddrLen);
#endif

    if(INVALID_SOCKET == cSock_)
    {
        printf("accept error socket:%d", (int)sockFd_);
    }
    else
    {
        NewUserJoin userJoin;
        SendDataToAll(&userJoin);
        g_clients.push_back(cSock_);
        printf("socket:%d 新客户端加入 socket:%d ,ip:%s \n",
               sockFd_, cSock_,inet_ntoa(cliAddr.sin_addr));
    }
    return cSock_;
}

void TcpService::Close()
{
    if(sockFd_ != INVALID_SOCKET)
    {
#ifdef _WIN32
#else
        for(int n = (int)g_clients.size() - 1; n >=0; n--)
        {
            close(g_clients[n]);
        }
        close(sockFd_);
#endif
    }
}

bool TcpService::OnRun()
{
    if(isRun())
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(sockFd_, &fdRead);
        FD_SET(sockFd_, &fdWrite);
        FD_SET(sockFd_, &fdExp);

        int maxSocket = sockFd_;

        //管理的链接数 将数据填入到就绪队列中
        for(int n = (int)g_clients.size() - 1; n >= 0; n--)
        {
            FD_SET(g_clients[n], &fdRead);
            if(maxSocket < g_clients[n])
            {
                maxSocket = g_clients[n];
            }
        }

        timeval time_ = {1, 0};
        int ret = select(maxSocket + 1, &fdRead, &fdWrite, &fdExp, &time_);
        if(ret < 0)
        {
            printf("select 任务结束\n");
            Close();
            return false;
        }

        if(FD_ISSET(sockFd_, &fdRead))
        {
            FD_CLR(sockFd_, &fdRead);
            Accept();
        }

        //循环遍历谁可读
        for(int n = (int)g_clients.size() - 1; n >= 0; n--)
        {
            if(FD_ISSET(g_clients[n], &fdRead))
            {
                if(-1 == RecvData(g_clients[n]))
                {
                    auto iter = g_clients.begin() + n;
                    if(iter != g_clients.end())
                    {
                        g_clients.erase(iter);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

bool TcpService::isRun()
{
    return sockFd_ != INVALID_SOCKET;
}

int TcpService::RecvData(int csock)
{
    char recvBuff[4096] = {};
    int recvLen = (int)recv(csock, recvBuff, sizeof(DataHeader), 0);
    DataHeader * pHeader = (DataHeader*)recvBuff;
    if(recvLen <= 0)
    {
        printf("socket:%d 退出 任务结束\n", csock);
        return -1;
    }
    recv(csock, recvBuff + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);
    OnNetMsg(csock, pHeader);
    return 0;
}

void TcpService::OnNetMsg(int csock, DataHeader * pHeader)
{
    switch(pHeader->cmd)
    {
    case CMD_LOGIN:
        {
            Login * pLogin = (Login*)pHeader;
            printf("收到client请求fd:%d logLen:%d userName:%s passWord:%s\n",
                   csock,pLogin->dataLength, pLogin->userName, pLogin->passWord);
        }
        break;
    case CMD_LOGOUT:
        {
            Logout * pLogout = (Logout*)pHeader;
            printf("client:%d dataLen:%d userName:%s\n",
                   csock, pLogout->dataLength, pLogout->userName);
        }
        break;
    default:
        {
            DataHeader head = {0, CMD_ERROR};
            send(csock, (char*)&head, sizeof(head), 0);
        }
        break;
    }
}

int TcpService::SendData(int csock, DataHeader * pHeader)
{
    if(isRun() &&  pHeader)
    {
        return send(csock, (const char*)pHeader, pHeader->dataLength, 0);
    }
    return Socket_Error;
}

void TcpService::SendDataToAll(DataHeader * pHeader)
{
    for(int n = g_clients.size() - 1; n >= 0; n--)
    {
        SendData(g_clients[n], pHeader);
    }
}
