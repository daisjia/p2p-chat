#include"Socket.h"


Socket::Socket(const char* ip, short port)
{
	_serfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serfd == -1)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "server create fail!" << endl;
		return;
	}
	char DateTime[_DATETIME_SIZE];
	GetDateTime(DateTime);
	cout << "===Author: Daijia===" << " " << DateTime << "server startup success!" << endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	int res = bind(_serfd, (struct sockaddr*) & saddr, sizeof(sockaddr));
	if (res == -1)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "server bind fail!" << endl;
		return;
	}

	res = listen(_serfd, 20);

	if (res == -1)
	{
		cout << "ser listen fail!" << endl;
		return;
	}
	_ip = ip;
	port = port;
}

string Socket::GetIp()
{
	return _ip;
}
int Socket::GetPort()
{
	return _port;
}

int Socket::Send(int fd, string message)
{
	int ret = send(fd, message.c_str(), message.size(), 0);
	return ret;
}
int Socket::Recv(int fd, string& message)
{
	int ret = recv(fd, (void*)message.c_str(), 1024, 0);
	return ret;
}

int Socket::Connect(struct sockaddr_in& caddr)
{
	socklen_t len = sizeof(caddr);
	int clifd = accept(_serfd, (struct sockaddr*) & caddr, &len);
	char DateTime[_DATETIME_SIZE];
	GetDateTime(DateTime);
	cout << "===Author: Daijia===" << " " << DateTime << "  ip: "<<inet_ntoa(caddr.sin_addr)<<"  port: "<<ntohs(caddr.sin_port)<<" connect success!" << endl;
	return clifd;
}

int Socket::GetFd()
{
	return _serfd;
}