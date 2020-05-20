/*************************************************************************
  @FileName: client.cpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @GitHub:   https://github.com/Moreys
  @Time:     Wed 20 May 2020 10:59:00 AM CST
 ************************************************************************/
#include "TcpClient.hpp"

#include <iostream>
#include <thread>

void cmdThread(TcpClient * client)
{
    while(true)
    {
        char cmdBuf[256] = {0};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf, "exit"))
        {
            printf("退出cmdThread线程\n");
            break;
        }
        else if(0 == strcmp(cmdBuf, "login"))
        {
            Login login;
            strcpy(login.userName, "momo");
            strcpy(login.passWord, "momo");
            client->SendData(&login);
        }
        else if(0 == strcmp(cmdBuf, "logout"))
        {
            Logout logout;
            strcpy(logout.userName, "momo");
            client->SendData(&logout);
        }
        else
        {
            printf("不支持该命令");
        }
    }
}

int main()
{
    TcpClient tcpClient;
    tcpClient.Connect("192.168.183.128", 9999);
    std::thread t1(cmdThread, &tcpClient);
    t1.detach();
    while(tcpClient.isRun())
    {
        tcpClient.OnRun();
        printf("空闲时间处理其他任务\n");
    }
    tcpClient.Close();
    printf("已退出\n");
    getchar();
    std::cout << "Hello world" << std::endl;
    return 0;
}

