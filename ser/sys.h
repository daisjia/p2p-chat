#pragma once
#include"Socket.h"
//#include"mysql.h"


enum TYPE
{
	REGISTER,
	LOGIN,
	GETFRIEND,
	GETIPPORT,
	EXIT
};

struct CliData
{
	int userid;
	int passwd;
	int state;
	string name;
	struct sockaddr_in caddr;
};

void DealCli(int fd, short event, void* arg);
void Connect(int fd, short event, void* arg);
void Register(int fd, string message, struct sockaddr_in& caddr);
void Login(int fd, string message, struct sockaddr_in& caddr);
void GetFriend(int fd, string message, struct sockaddr_in& caddr);
void Exit(int fd, string message, struct sockaddr_in& caddr);
void GetIpPort(int fd, string message, struct sockaddr_in& caddr);
void* pthread_run(void*);

class Sys
{
public:
	Sys(const char* ip, const int port);
	void Run();
	struct event_base* base;
	map<int, struct event_base*> clibase;
	static map<int, CliData*> clidata;
	static map<int, int> user;
	static Socket* _ser;
};



























#if 0
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
#include"log.h"
#include<json/json.h>
#include"process.h"
#include<iostream>
#include <fcntl.h>
#include<unistd.h>
using namespace std;

struct HEAD
{
	int size;
	int port;
	char ip[20];
};

void listen_cb(int fd, short event, void* arg);


enum TYPE
{
	REGISTER,
	LOGIN,
	GETFRIEND,
	EXIT
};

map<int, struct event*> ents;

void Run(int fd, short event, void* arg)
{
	string json = (char*)arg;
	Json::Value val;
	Json::Reader read;

	if (-1 == read.parse(json, val))
	{
		LOGE("json prase fail!");
		return;
	}

	string ip = val["ip"].asString();
	int port = val["port"].asInt();

	//cout << ip << "   " << port << endl;

	char buff[2048] = { 0 };

	int ret = recv(fd, buff, 2047, 0);
	if (ret <= 0)
	{
		LOGE("cli exit!");
		//event_free(ents[fd]);
		//close(fd);
		return;
	}
	Json::Value val2;
	//cout << json << endl;
	if (read.parse(json, val2) == -1)
	{
		LOGE("json parse fail!");
		return;
	}

	int type = val2["type"].asInt();

	switch (type)
	{
	case REGISTER: Register(fd, ip, port, buff);  break;
	case LOGIN: Login(fd, ip, port, buff); break;
	case GETFRIEND: GetFriend(fd, ip, port, buff); break;
	case EXIT: Exit(fd, ip, port, buff); break;
	default:
		break;
	}
}

void* pthread_run(void* arg)
{
	struct event_base* base = (struct event_base*)arg;
	event_base_dispatch(base);
}

class Socket
{
public:
	Socket(const char* ip, short port)
	{
		serfd = socket(AF_INET, SOCK_STREAM, 0);
		if (serfd == -1)
		{
			LOGE("ser socket fd create fail");
			return;
		}
		LOGD("ser socket fd create success!");

		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(port);
		saddr.sin_addr.s_addr = inet_addr(ip);

		int res = bind(serfd, (struct sockaddr*) & saddr, sizeof(sockaddr));
		if (res == -1)
		{
			LOGE("ser bind fail!");
			return;
		}

		res = listen(serfd, 20);

		if (res == -1)
		{
			LOGE("ser listen fail!");
			return;
		}

		base = event_base_new();
		clibase = event_base_new();

		struct event* env = event_new(base, serfd, EV_READ | EV_PERSIST, listen_cb, this);
		if (env == NULL)
		{
			LOGE("ser event new fail");
		}

		event_add(env, NULL);
		events.insert(make_pair(serfd, env));
		pthread_t id;
		//res = pthread_create(&id, NULL, pthread_run, this->base);
		if (res != 0)
		{
			LOGE("ser thread create fail!");
			return;
		}
	}

	void Start()
	{
		pthread_t id;
		pthread_create(&id, NULL, pthread_run, clibase);
		event_base_dispatch(base);
	}

private:
	int serfd;
	struct event_base* base;
	struct event_base* clibase;

	friend void listen_cb(int fd, short event, void* arg);
};

void listen_cb(int fd, short event, void* arg)
{
	Socket* mythis = (Socket*)arg;
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);

	int clifd = accept(fd, (struct sockaddr*) & caddr, &len);

	if (-1 == clifd)
	{
		LOGE("accept fail");
		return;
	}

	string ip = inet_ntoa(caddr.sin_addr);
	short port = ntohs(caddr.sin_port);
	Json::Value val;
	val["ip"] = ip;
	val["port"] = port;

	struct event* ev = event_new(mythis->clibase, clifd, EV_READ | EV_PERSIST, Run, (void*)val.toStyledString().c_str());

	if (ev == NULL)
	{
		LOGE("event new fail!");
	}

	event_add(ev, NULL);
	events.insert(make_pair(clifd, ev));
}


#endif