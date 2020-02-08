#include"mysql.h"

Mysql::Mysql()
{
	mpcon = mysql_init((MYSQL*)0);
	if (mpcon == NULL)
	{
		cout << "mpcon == NULL ==>> Mysql()" << endl;
		return;
	}

	if (!mysql_real_connect(mpcon, "127.0.0.1", "root", "123456", NULL, 3306, NULL, 0))	//³É¹¦·µ»Ø0
	{
		cout << "mysql connect fail ==>> Mysql()" << endl;
		return;
	}

	if (mysql_select_db(mpcon, "p2p"))
	{
		cout << "database select fail ==>> Mysql()" << endl;
		return;
	}
}

Mysql::~Mysql()
{
	if (NULL != mp_res)
	{
		mysql_free_result(mp_res);
	}
	mysql_close(mpcon);
}
