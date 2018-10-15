#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"

int main(int argc, char** argv)
{
    //创建mysql连接
	MYSQL* pMysql = mysql_init(NULL);

	//连接数据库
	MYSQL* pRet = mysql_real_connect(pMysql, "127.0.0.1", "root", "r00t", "demo", 0, NULL, 0);
	if (pRet == NULL)
	{
		printf("%s\n", mysql_error(pMysql));
	}

	//设置连接编码
	mysql_query(pMysql, "set names utf8");

	//增
#if 0
	int nRet = mysql_query(pMysql, "insert into test1 (name, sex, age) values('hello', 1, 20)");
	if (nRet != 0) {
		printf("%s\n", mysql_error(pMysql));
	}
#endif

	//删
#if 0
	int nRet = mysql_query(pMysql, "delete from test1 where name = 'hello'");
	if (nRet != 0) {
		printf("%s\n", mysql_error(pMysql));
	}
#endif

	//改
#if 0
	int nRet = mysql_query(pMysql, "update test1 set name = 'hello123' where name = 'hello'");
	if (nRet != 0) {
		printf("%s\n", mysql_error(pMysql));
	}

	int nLinesNum = mysql_affected_rows(pMysql);
	printf("lines : %d\n", nLinesNum);
#endif

	//查
#if 1
	int nRet = mysql_query(pMysql, "select * from test1");
	if (nRet != 0) {
		printf("%s\n", mysql_error(pMysql));
	}
	else
	{
		MYSQL_RES* pRes = mysql_store_result(pMysql);
		MYSQL_ROW row;

		while (row = mysql_fetch_row(pRes))
		{
			printf("%s, %s, %s, %s\n", row[0], row[1], row[2], row[3]);
		}
	}
#endif

	//关闭mysql连接
	mysql_close(pMysql);

	system("pause");
    return 0;
}