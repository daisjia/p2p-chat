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
#include"log.h"
#include<json/json.h>
#include"mysql.h"
#include<iostream>
using namespace std;

struct CliData
{
	int userid;
	int passwd;
	int state;
	string name;
	struct sockaddr_in caddr;
};

map<int, struct event*> events;
Mysql mysql;

void Register(int fd, string &message, struct sockaddr_in &caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		LOGE("json prase error!");
		return;
	}
	int userid = val["userid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it != Sys::clidata.end())
	{
		cout << "user exist,register fail!" << endl;
		return;
	}
	int passwd = val["passwd"].asInt();
	string name = val["name"].asString();
	CliData* data = new CliData();
	data->name = name;
	data->caddr = caddr;
	data->passwd = passwd;
	data->state = 0;
	data->userid = userid;
	Sys::clidata.insert(make_pair(userid, data));

	Sys::_ser->Send(fd, "t");


//	Json::Value val;
//	Json::Reader read;
//	if (read.parse(json, val) == -1)
//	{
//		LOGE("json parse error!");
//		return;
//	}
//
//
//	cout << json.size() << endl;
//
//	int id = val["id"].asInt();
//	char cmd[100];
//	sprintf(cmd, "select id from chat where id = %d;", id);
//	if (mysql_real_query(mysql.mpcon, cmd, strlen(cmd)))
//	{
//		LOGE("select error!");
//		return;
//	}
//
//	mysql.mp_res = mysql_store_result(mysql.mpcon);
//	if ((mysql.mp_row = mysql_fetch_row(mysql.mp_res)))	//ÓÃ»§´æÔÚ
//	{
//		send(fd, "false", 6, 0);
//		return;
//	}
//
//	string name = val["name"].asString();
//	//cout << "name : " << name.c_str() << endl;
//	string passwd = val["passwd"].asString();
//
//	char cmd2[100] = { 0 };
//
//	//id, name, ip, port, state
//	sprintf(cmd2, "insert into chat value (%d, \"%s\", \"%s\", %d, %d);", id, name.c_str(), ip.c_str(), port, 0);
//	mysql_real_query(mysql.mpcon, cmd2, strlen(cmd2));
//	send(fd, "true", 5, 0);
//	
//	char buff[100] = { 0 };
////	sprintf(buff, "user: %s register success!", name.c_str());
//	LOGD(buff);
}

void Login(int fd, string& message, struct sockaddr_in& caddr)
{


}

void GetFriend(int fd, string& message, struct sockaddr_in& caddr)
{


}

void Exit(int fd, string& message, struct sockaddr_in& caddr)
{


}

