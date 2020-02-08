#include<iostream>
using namespace std;
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<string>
#include<json/json.h>
#include<stdio.h>
#include<pthread.h>
#include <fcntl.h>
#include<errno.h>

enum TYPE
{
	REGISTER,
	LOGIN,
	GETFRIEND,
	GETIPPORT,
	EXIT
};

struct HEAD
{
	int size;
	int port;
	char ip[20];
};

void Register(int fd)
{
	int userid;
	string name;
	int passwd;
	cout << "input userid: ";
	cin >> userid;
	cout << "input name: ";
	cin >> name;
	cout << "input passwd: ";
	cin >> passwd;
	Json::Value val;
	val["userid"] = userid;
	val["type"] = REGISTER;
	val["name"] = name;
	val["passwd"] = passwd;
	send(fd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024] = { 0 };
	int res = recv(fd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}

	if (*buff == 't')
	{
		cout << "register success!" << endl;
	}
	else
	{
		cout << buff << endl;
	}
}

void Login(int fd)
{
	int userid;
	int passwd;
	cout << "input userid: ";
	cin >> userid;
	cout << "input passwd: ";
	cin >> passwd;
	Json::Value val;
	val["userid"] = userid;
	val["type"] = LOGIN;
	val["passwd"] = passwd;
	send(fd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024] = { 0 };
	int res = recv(fd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}

	if (*buff == 't')
	{
		cout << "login success!" << endl;
	}
	else
	{
		cout << buff << endl;
	}
}

void GetFriend(int fd)
{
	int userid;
	cout << "input userid: ";
	cin >> userid;

	Json::Value val;
	val["userid"] = userid;
	val["type"] = GETFRIEND;
	send(fd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024 * 1024] = { 0 };
	int res = recv(fd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}

	cout << buff << endl;
}

void GetIpPort(int fd)
{
	int userid;
	cout << "input userid: ";
	cin >> userid;
	int friendid;
	cout << "your friend id: ";
	cin >> friendid;

	Json::Value val;
	val["userid"] = userid;
	val["friendid"] = friendid;
	val["type"] = GETIPPORT;
	send(fd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024 * 1024] = { 0 };
	int res = recv(fd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}

	string str = buff;
	cout << endl;
	cout << buff << endl;
	Json::Value val1;
	Json::Reader read;
	if (-1 == read.parse(str, val1));
	{
		cerr << "json prase fail;errno:" << errno << endl;
	}
	cout << "userid: " << val1["userid"].asInt() << "  name: " << val1["name"].asString() << "   ip: " << val1["ip"].asString() << "   port: " << val1["port"].asInt() << endl;

}

int main()
{
	cout << "please input port: ";
	int port;
	cin >> port;
	cout << endl;
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	int res = connect(fd, (struct sockaddr*) & saddr, sizeof(saddr));
	if (res == -1)
	{
		cout << "connect error!" << endl;
		return 0;
	}

	while(1)
	{
		cout << "=====register : 1" << endl;
		cout << "=====login: 2" << endl;
		cout << "=====getfriend: 3" << endl;
		cout << "=====getipport: 4" << endl;
		int choice;
		cout << "please choice: ";
		cin >> choice;
		if (choice == 1)
		{
			Register(fd);
		}
		else if(choice == 2)
		{
			Login(fd);
		}
		else if (choice == 3)
		{
			GetFriend(fd);
		}
		else if (choice == 4)
		{
			GetIpPort(fd);
		}
	}
	return 0;
}

