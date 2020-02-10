#include "client.h"

map<int, struct event*> myevents;

void* pthread_run(void* arg)
{
	int pair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
	{
		cout << "socketpair create fail ==>> void TcpServer::GetSockPair()" << endl;
	}
	struct event_base* base = (struct event_base*)arg;
	struct event* env = event_new(base, pair[0], EV_READ | EV_PERSIST, Deal, NULL);
	event_add(env, NULL);
	event_base_dispatch(base);
}

void Deal(int fd, short event, void* arg)
{
	char message[1024 * 1024] = { 0 };
	int res = recv(fd, message, 1024 * 1024, 0);
	if (res <= 0)
	{
		event_free(myevents[fd]);
		myevents.erase(fd);
		close(fd);
		cout << "cli exit" << endl;
		PutMenu();
		return;
	}
	cout << endl;
	cout << message << endl;
	PutMenu();
}

void Connect(int fd, short event, void* arg)
{
	Client* cli = (Client*)arg;
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int clifd = accept(fd, (struct sockaddr*) & caddr, &len);
	if (clifd == -1)
	{
		cout << "accept fail!" << endl;
		return;
	}
	static int i = 0;
	struct event* ev = event_new(cli->base[0], clifd, EV_READ | EV_PERSIST, Deal, (void*)&caddr);

	if (ev == NULL)
	{
		cout<<"event new fail!" << endl;
		return;
	}
	myevents.insert(make_pair(clifd, ev));
	event_add(ev, NULL);
}

Client::Client(string ip, int port)
{
	myip = ip;
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_addr.s_addr = inet_addr(ip.c_str());
	saddr.sin_port = htons(port);
	saddr.sin_family = AF_INET;
	clifd = socket(AF_INET, SOCK_STREAM, 0);
	myfd = socket(AF_INET, SOCK_STREAM, 0);

	int res = connect(clifd, (struct sockaddr*) & saddr, sizeof(saddr));
	if (res == -1)
	{
		cout << "connect error!" << endl;
		return;
	}
	base[0] = event_base_new();
	base[1] = event_base_new();

	for (int i = 0; i < 2; ++i)
	{
		pthread_t id;
		pthread_create(&id, NULL, pthread_run, base[i]);
	}
}

void Client::Run()
{
	while (1)
	{
		cout << "     ==========================" << endl;
		cout << "     *..... REGISTER : 1 .....*" << endl;
		cout << "     *....... LOGIN: 2 .......*" << endl;
		cout << "     *....... EXIT: 3  .......*" << endl;
		cout << "     ==========================" << endl;
		int choice;
		cout << "-->please choice: ";
		cin >> choice;
		if (choice == 1)
		{
			Register();
		}
		else if (choice == 2)
		{
			Login();
		}
		else if (choice == 3)
		{
			Exit();
		}
		else
		{
			cout << "input error!" << endl;
		}
	}
}

void Client::Register()
{
	int userid;
	string name;
	int passwd;
	cout << "-->input userid: ";
	cin >> userid;
	cout << "-->input name: ";
	cin >> name;
	cout << "-->input passwd: ";
	cin >> passwd;
	myid = userid;
	myname = name;
	mypasswd = passwd;
	Json::Value val;
	Json::Reader read;
	val["userid"] = userid;
	val["type"] = REGISTER;
	val["name"] = name;
	val["passwd"] = passwd;
	send(clifd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024] = { 0 };
	int res = recv(clifd, buff, 1023, 0);
	string message = buff;
	Json::Value rsp;
	if (-1 == read.parse(message, rsp))
	{
		cout << "json parse error!" << endl;
	}
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}

	if (rsp["res"].asBool())
	{
		myport = rsp["port"].asInt();
		struct sockaddr_in saddr;
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_addr.s_addr = inet_addr(myip.c_str());
		saddr.sin_port = htons(myport);
		saddr.sin_family = AF_INET;
		res = bind(myfd, (struct sockaddr*) & saddr, sizeof(sockaddr));
		if (res == -1)
		{
			cout << "bind error!" << endl;
			return;
		}
		struct event* env = event_new(base[0], myfd, EV_READ | EV_PERSIST, Connect, this);
		listen(myfd, 20);
		event_add(env, NULL);

		cout << rsp["message"].asString()<< endl;
	}
	else
	{
		cout << rsp["message"].asString() << endl;
	}
}

void Client::Login()
{
	int userid;
	int passwd;
	cout << "-->input userid: ";
	cin >> userid;
	cout << "-->input passwd: ";
	cin >> passwd;
	Json::Value val;
	val["userid"] = userid;
	val["type"] = LOGIN;
	val["passwd"] = passwd;
	send(clifd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024] = { 0 };
	int res = recv(clifd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}
	string str = buff;
	Json::Value rsp;
	Json::Reader read;
	read.parse(str, rsp);

	if (rsp["res"].asBool())
	{
		cout << rsp["message"].asString() << endl;
		Start();
	}
	else
	{
		cout << rsp["message"].asString() << endl;
	}
}

void Client::Start()
{
	while (1)
	{
		PutMenu();
		int choice;
		cin >> choice;
		if (choice == 1)
		{
			GetOnlineFriend();
		}
		else if (choice == 2)
		{
			SendMessage();
		}
		else if (choice == 3)
		{
			return;
		}
		else
		{
			cout << "input error!" << endl;
		}
	}
}

void Client::GetOnlineFriend()
{
	Json::Value val;
	val["userid"] = myid;
	val["type"] = GETFRIEND;
	send(clifd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024 * 1024] = { 0 };
	int res = recv(clifd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return;
	}
	Json::Value rsp;
	Json::Reader read;
	string str = buff;
	read.parse(buff, rsp);
	
	if (rsp["res"].asBool())
	{
		cout << rsp["message"].asString() << endl;
	}
	else
	{
		cout << rsp["message"].asString() << endl;
	}
}

void Client::SendMessage()
{
	GetOnlineFriend();
	int friendid;
	cout << "-->your friend id: ";
	cin >> friendid;

	map<int, int>::iterator it = myfriend.find(friendid);
	int fd = 0;
	if (it == myfriend.end())
	{
		fd = GetSockFd(friendid);
		if (fd == -1)
		{
			return;
		}
	}
	else
	{
		fd = it->second;
	}
	
	char buff[1024 * 1024] = { 0 };
	char mes[1024] = { 0 };
	cout << "-->please input message: ";
	cin >> mes;
	Json::Value req;
	req["id"] = myid;
	req["name"] = myname;
	req["message"] = mes;
	int res = send(fd, req.toStyledString().c_str(), req.toStyledString().size(), 0);

	if (res <= 0)
	{
		myfriend.erase(friendid);
		close(myfriend[friendid]);
	}
}

int Client::GetSockFd(int friendid)
{
	Json::Value val;
	val["userid"] = myid;
	val["friendid"] = friendid;
	val["type"] = GETIPPORT;
	send(clifd, val.toStyledString().c_str(), val.toStyledString().size(), 0);
	char buff[1024 * 1024] = { 0 };
	int res = recv(clifd, buff, 1023, 0);
	if (res == -1)
	{
		cout << "recv error!" << endl;
		return -1;
	}

	string str = buff;
	cout << endl;
	Json::Value rsp;
	Json::Reader read;
	read.parse(str, rsp);

	if (!rsp["res"].asBool())
	{
		cout << rsp["message"].asString() << endl;
		return -1;
	}
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_addr.s_addr = inet_addr(rsp["ip"].asString().c_str());
	saddr.sin_port = htons(rsp["port"].asInt());
	saddr.sin_family = AF_INET;
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	res = connect(fd, (struct sockaddr*) & saddr, sizeof(saddr));
	if (res == -1)
	{
		cout << "connect error!" << endl;
		return -1;
	}

	myfriend.insert(make_pair(friendid, fd));
	struct event* env = event_new(base[1], fd, EV_READ | EV_PERSIST, Deal, this);
	event_add(env, NULL);
	return fd;
}


void Client::Exit()
{
	close(clifd);
}

