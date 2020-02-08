#include"sys.h"

map<int, CliData*>Sys::clidata;
Socket* Sys::_ser;
//map<int, struct event_base*> Sys::clibase;
//Mysql mysql;
map<int, struct event*> myevents;

void DealCli(int fd, short event, void* arg)
{
	cout << "dealcli" << endl;
	struct sockaddr_in caddr = *(struct sockaddr_in*)arg;
	string message;
	int res = Sys::_ser->Recv(fd, message);
	//cout << message << endl;
	if (res <= 0)
	{
		cout << "cli exit!" << endl;
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
			cout << "json prase error!" << endl;
			return;
		}
		int type = val["type"].asInt();

		switch (type)
		{
		case REGISTER: Register(fd, message, caddr);  break;
		case LOGIN: Login(fd, message, caddr); break;
		case GETFRIEND: GetFriend(fd, message, caddr); break;
		case EXIT: Exit(fd, message, caddr); break;
		default:
			break;
		}
	}
}

void Connect(int fd, short event, void* arg)
{
	cout << "connect" << endl;
	Sys* sys = (Sys*)arg;
	struct sockaddr_in caddr;
	int clifd = Sys::_ser->Connect(caddr);
	cout << "===============" << clifd << endl;
	if (clifd == -1)
	{
		cout << "ser connect fail" << endl;
		return;
	}
	static int i = 0;
	struct event* ev = event_new(sys->clibase[i % 4], clifd, EV_READ | EV_PERSIST, DealCli, (void*)&caddr);
	myevents.insert(make_pair(clifd, ev));
	i++;
	if (ev == NULL)
	{
		cout<<"event new fail!"<<endl;
	}

	event_add(ev, NULL);
	cout << "connect endl"<<endl;
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
	cout << "1111111111111111111111" << endl;
	int pair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
	{
		cout << "socketpair create fail ==>> void TcpServer::GetSockPair()" << endl;
	}
	struct event_base* base = (struct event_base*)arg;
	struct event* env = event_new(base, pair[0], EV_READ | EV_PERSIST, Connect, NULL);
	event_add(env, NULL);
	event_base_dispatch(base);
	cout << "22222222222222222" << endl;
}

void Register(int fd, string& message, struct sockaddr_in& caddr)
{
	Json::Value val;
	Json::Reader read;
	if (-1 == read.parse(message, val))
	{
		cout<<"json prase error!"<<endl;
		return;
	}
	int userid = val["userid"].asInt();
	map<int, CliData*>::iterator it = Sys::clidata.find(userid);
	if (it != Sys::clidata.end())
	{
		cout << "user exist,register fail!" << endl;
		Sys::_ser->Send(fd, "f");
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