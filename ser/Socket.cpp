#include"Socket.h"

Socket::Socket(const char* ip, short port)
{
	_serfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serfd == -1)
	{
		cout<<"ser socket fd create error"<<endl;
		return;
	}
	cout << "ser socket fd create success!" << endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	int res = bind(_serfd, (struct sockaddr*) & saddr, sizeof(sockaddr));
	if (res == -1)
	{
		cout<<"ser bind error!"<<endl;
		return;
	}

	res = listen(_serfd, 20);

	if (res == -1)
	{
		cout << "ser listen error!" << endl;
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
	cout << message.c_str() << endl;
	return ret;
}

int Socket::Connect(struct sockaddr_in& caddr)
{
	socklen_t len = sizeof(caddr);
	int clifd = accept(_serfd, (struct sockaddr*) & caddr, &len);
	return clifd;
}

int Socket::GetFd()
{
	return _serfd;
}