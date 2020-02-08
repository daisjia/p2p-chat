#include<iostream>
using namespace std;
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<json/json.h>
#include<stdio.h>
#include<pthread.h>
#include <fcntl.h>

enum TYPE
{
	REGISTER,
	LOGIN,
	GETFRIEND,
	EXIT
};

struct HEAD
{
	int size;
	int port;
	char ip[20];
};

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

	for (int i = 0; i < 100; ++i)
	{
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		
		int res = connect(fd, (struct sockaddr*) & saddr, sizeof(saddr));
		if (res == -1)
		{
			cout << "connect error!" << endl;
			return 0;
		}
		char buff1[10] = { 0 };
		sprintf(buff1, "jiadai%d", i);
		string name = buff1;
		string pw("123456");
		Json::Value val;
		val["userid"] = i;
		val["type"] = REGISTER;
		val["name"] = name;
		val["passwd"] = pw;
		
		cout << val.toStyledString() << endl;
		sleep(1);
		res = send(fd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
		if (res == -1)
		{
			cout << "send error!" << endl;
			return 0;
		}

		char buff[2] = { 0 };
		res = recv(fd, buff, 2, 0);
		if (res == -1)
		{
			cout << "recv error!" << endl;
			return 0;
		}
		cout << buff << endl;
		close(fd);
	}
	return 0;
}

