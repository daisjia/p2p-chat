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

enum TYPE
{
	REGISTER,
	LOGIN,
	GETFRIEND,
	GETIPPORT,
	EXIT
};
void Connect(int fd, short event, void* arg);
void* pthread_run(void*);
void Deal(int fd, short event, void* arg);

void PutMenu()
{
	cout << "     ==========================" << endl;
	cout << "     *...GetOnlineFriend : 1..*" << endl;
	cout << "     *.... SendMessage: 2 ....*" << endl;
	cout << "     *....... EXIT: 3  .......*" << endl;
	cout << "     ==========================" << endl;
	cout << "-->please choice: ";
}

class Client
{
public:
	Client(string ip, int port);
	void Run();
	void Register();
	void Login();
	void GetOnlineFriend();
	void SendMessage();
	void Exit();
	void Start();
	int GetSockFd(int friendid);

private:
	int clifd;
	int myfd;
	int myport;
	string myip;
	struct event_base* base[2];
	friend void Connect(int fd, short event, void* arg);	
	int myid;
	string myname;
	int mypasswd;

	map<int, int> myfriend;
};