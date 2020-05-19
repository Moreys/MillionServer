/*************************************************************************
  @FileName: service.cpp
  @Author:   morey
  @Email:    zymorey@gmail.com
  @Time:     Sat 16 May 2020 03:20:55 AM PDT
 ************************************************************************/
#include "TcpService.hpp"
#include <iostream>

int main()
{
    TcpService service;
    service.InitSocket();
    service.Bind(nullptr, 9999);
    service.Listen(5);
    while(service.isRun())
    {
        service.OnRun();
    }
    service.Close();
    printf("退出\n");
    getchar();
    std::cout << "Hello world" << std::endl;
    return 0;
}

