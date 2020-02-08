#include<iostream>
using namespace std;
#include"sys.h"

int main()
{
	int port;
	cout << "please input port: ";
	cin >> port;
	cout << endl;
	Sys* ser = new Sys("127.0.0.1", port);
	ser->Run();
	//delete ser;
	return 0;
}