#include"sys.h"

map<int, CliData*>Sys::clidata;
Socket* Sys::_ser;
//map<int, struct event_base*> Sys::clibase;
//Mysql mysql;
map<int, struct event*> myevents;

void DealCli(int fd, short event, void* arg)
{
	struct sockaddr_in caddr = *(struct sockaddr_in*)arg;
	char message[1024] = { 0 };
	int res = recv(fd, message, 1024, 0);
	if (res <= 0)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "client exit!" << endl;
		event_free(myevents[fd]);
		myevents.erase(fd);
		close(fd);
	}
	else
	{
		Json::Value val;
		Json::Reader read;
		if (-1 == read.parse(message, val))
		{
			char DateTime[_DATETIME_SIZE];
			GetDateTime(DateTime);
			cout << "===Author: Daijia===" << " " << DateTime << "json parse fail!" << endl;
			return;
		}
		int type = val["type"].asInt();
		
		switch (type)
		{
		case REGISTER: Register(fd, message, caddr);  break;
		case LOGIN: Login(fd, message, caddr); break;
		case GETFRIEND: GetFriend(fd, message, caddr); break;
		case GETIPPORT: GetIpPort(fd, message, caddr); break;
		case EXIT: Exit(fd, message, caddr); break;
		default:
			break;
		}
	}
}

void Connect(int fd, short event, void* arg)
{
	Sys* sys = (Sys*)arg;
	struct sockaddr_in caddr;
	int clifd = Sys::_ser->Connect(caddr);
	if (clifd == -1)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "connect fail!" << endl;
		return;
	}
	static int i = 0;
	struct event* ev = event_new(sys->clibase[i % 4], clifd, EV_READ | EV_PERSIST, DealCli, (void*)&caddr);
	myevents.insert(make_pair(clifd, ev));
	i++;
	if (ev == NULL)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "event new fail!" << endl;
	}

	event_add(ev, NULL);
}

Sys::Sys(const char* ip, const int port)
{
	_ser = new Socket(ip, port);
	base = event_base_new();
	clibase.insert(make_pair(0, event_base_new()));
	clibase.insert(make_pair(1, event_base_new()));
	clibase.insert(make_pair(2, event_base_new()));
	clibase.insert(make_pair(3, event_base_new()));
	struct event* env = event_new(base, _ser->GetFd(), EV_READ | EV_PERSIST, Connect, this);
	event_add(env, NULL);
}

void Sys::Run()
{
	pthread_t id;
	for (int i = 0; i < 4; ++i)
	{
		pthread_create(&id, NULL, pthread_run, clibase[i]);
	}
	event_base_dispatch(base);
}

void* pthread_run(void*arg)
{
	int pair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
	{
		cout << "socketpair create fail ==>> void TcpServer::GetSockPair()" << endl;
	}
	struct event_base* base = (struct event_base*)arg;
	struct event* env = event_new(base, pair[0], EV_READ | EV_PERSIST, Connect, NULL);
	event_add(env, NULL);
	event_base_dispatch(base);
}

void Register(int fd, string message, struct sockaddr_in& caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "json parse fail!" << endl;
		return;
	}
	int userid = val["userid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it != Sys::clidata.end())
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " register fail!" << endl;
		Sys::_ser->Send(fd, "user exist!");
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
	char DateTime[_DATETIME_SIZE];
	GetDateTime(DateTime);
	cout << "===Author: Daijia===" << " Time: " << DateTime << "  userid: "<<userid << " register success!" << endl;

	//	Json::Value val;
	//	Json::Reader read;
	//	if (read.parse(json, val) == -1)
	//	{
	//		LOGE("json parse fail!");
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
	//		LOGE("select fail!");
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

void Login(int fd, string message, struct sockaddr_in& caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "json parse fail!" << endl;
		return;
	}
	int userid = val["userid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it == Sys::clidata.end())
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " not exist!" << endl;
		Sys::_ser->Send(fd, "user not exist!");
		return;
	}
	int passwd = val["passwd"].asInt();
	if (passwd == it->second->passwd)
	{
		if (it->second->state == 1)
		{
			Sys::_ser->Send(fd, "login fail, user alreadly online!");
			return;
		}
		it->second->state = 1;
		Sys::_ser->Send(fd, "t");
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " Time: " << DateTime << "  userid: " << userid << " login success!" << endl;
	}
	else
	{
		Sys::_ser->Send(fd, "passwd error!");
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " Time: " << DateTime << "  userid: " << userid << " login fail!" << endl;
	}
}

void GetFriend(int fd, string message, struct sockaddr_in& caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "json parse fail!" << endl;
		return;
	}
	int userid = val["userid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it == Sys::clidata.end())
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " getfriend fail!" << endl;
		Sys::_ser->Send(fd, "please register first!");
		return;
	}
	if(it->second->state == 0)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " getfriend fail!" << endl;
		Sys::_ser->Send(fd, "please login first!");
		return;
	}
	
	char buff[1024 * 1024] = { 0 };
	it = Sys::clidata.begin();
	for (; it != Sys::clidata.end(); it++)
	{
		if (it->second->state == 1)
		{
			char buff1[100] = { 0 };
			sprintf(buff1, "userid: %d   name: %s\n", it->second->userid, it->second->name.c_str());
			strcat(buff, buff1);
		}
	}

	Sys::_ser->Send(fd, buff);
	char DateTime[_DATETIME_SIZE];
	GetDateTime(DateTime);
	cout << "===Author: Daijia===" << " Time: " << DateTime << "  userid: " << userid << " getfriend success!" << endl;
}

void GetIpPort(int fd, string message, struct sockaddr_in& caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "json parse fail!" << endl;
		return;
	}
	int userid = val["userid"].asInt();
	int friendid = val["friendid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it == Sys::clidata.end())
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " GetIpPort fail!" << endl;
		Sys::_ser->Send(fd, "please register first!");
		return;
	}
	if (it->second->state == 0)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " GetIpPort fail!" << endl;
		Sys::_ser->Send(fd, "please login first!");
		return;
	}

	it = Sys::clidata.find(friendid);
	if (it == Sys::clidata.end())
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " GetIpPort fail!" << endl;
		Sys::_ser->Send(fd, "your friend not exist!");
		return;
	}

	if (it->second->state == 0)
	{
		char DateTime[_DATETIME_SIZE];
		GetDateTime(DateTime);
		cout << "===Author: Daijia===" << " " << DateTime << "  userid: " << userid << " GetIpPort fail!" << endl;
		Sys::_ser->Send(fd, "your friend not online!");
		return;
	}

	Json::Value vall;
	vall["userid"] = it->second->userid;
	vall["name"] = it->second->name;
	vall["ip"] = inet_ntoa(it->second->caddr.sin_addr);
	vall["port"] = ntohs(it->second->caddr.sin_port);

	Sys::_ser->Send(fd, vall.toStyledString());
	char DateTime[_DATETIME_SIZE];
	GetDateTime(DateTime);
	cout << "===Author: Daijia===" << " Time: " << DateTime << "  userid: " << userid << " GetIpPort success!" << endl;
}

void Exit(int fd, string message, struct sockaddr_in& caddr)
{


}