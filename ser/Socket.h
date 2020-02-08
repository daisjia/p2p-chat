#pragma once
#include<vector>
#include<map>
#include<stdlib.h>
#include<memory.h>
#include<string>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<json/json.h>
#include<sys/types.h>
#include<event.h>
#include<json/json.h>
#include<iostream>
#include <fcntl.h>
#include<unistd.h>
#include"mytime.h"
using namespace std;

class Socket
{
public:
	Socket(const char* ip, const short port);
	int Connect(struct sockaddr_in& caddr);
	string GetIp();
	int GetPort();
	int GetFd();
	int Send(int fd, string message);
	int Recv(int fd, string& message);
private:
	string _ip;
	int _port;
	int _serfd;
};
