#pragma once
#include<mysql/mysql.h>
#include<iostream>
using namespace std;
#include<string>
#include<exception>

class Mysql
{
public:
	Mysql();
	~Mysql();

	MYSQL* mpcon;	//���ݿ�
	MYSQL_RES* mp_res;	//����ֵ
	MYSQL_ROW mp_row;	//ÿһ��

private:

};
