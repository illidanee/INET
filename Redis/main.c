#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "hiredis.h"
#define NO_QFORKIMPL
#include "Win32_Interop\win32fixes.h"

int main(int argc, char** argv)
{
	//Á¬½Óredis
	struct timeval timeout = { 1, 500000 };
	struct redisContext* pRC = redisConnectWithTimeout("47.94.18.115", 6379, timeout);
	if (pRC->err)
	{
		printf("Connect error: %s\n", pRC->errstr);
		goto FLAG_END;
	}

	struct redisReply* pReply = NULL;
	//Ö´ÐÐÃüÁî£º select 10
	pReply = redisCommand(pRC, "select %d", 10);
	if (pReply == NULL)
	{
		printf("Command error: %s\n", pRC->errstr);
		goto FLAG_END;
	}
	else
	{
		printf("%d, %s\n", pReply->type, pReply->str);
		freeReplyObject(pReply);
	}

	//Ö´ÐÐÃüÁî£ºzadd world_rank 3000 xt
	pReply = redisCommand(pRC, "zadd world_rank 3000 xt");
	if (pReply == NULL)
	{
		printf("Command error: %s\n", pRC->errstr);
		goto FLAG_END;
	}
	else
	{
		printf("%d, %d\n", pReply->type, pReply->integer);
		freeReplyObject(pReply);
	}

	//Ö´ÐÐÃüÁî£ºzadd world_rank 2000 xm
	pReply = redisCommand(pRC, "zadd world_rank 2000 xm");
	if (pReply == NULL)
	{
		printf("Command error: %s\n", pRC->errstr);
		goto FLAG_END;
	}
	else
	{
		printf("%d, %d\n", pReply->type, pReply->integer);
		freeReplyObject(pReply);
	}

	//Ö´ÐÐÃüÁî£ºzadd world_rank 2600 xh
	pReply = redisCommand(pRC, "zadd world_rank 2600 xh");
	if (pReply == NULL)
	{
		printf("Command error: %s\n", pRC->errstr);
		goto FLAG_END;
	}
	else
	{
		printf("%d, %d\n", pReply->type, pReply->integer);
		freeReplyObject(pReply);
	}

	//Ö´ÐÐÃüÁî£ºzrange world_rank 0 10 withscores
	pReply = redisCommand(pRC, "zrange world_rank 0 10 withscores");
	if (pReply == NULL)
	{
		printf("Command error: %s\n", pRC->errstr);
		goto FLAG_END;
	}
	else
	{
		if (pReply->type == REDIS_REPLY_ARRAY)
		{
			for (int i = 0; i < pReply->elements; ++i)
			{
				printf("%d, %s\n", pReply->type, pReply->element[i]->str);
			}
		}
		freeReplyObject(pReply);
	}

	//Ê§°Ü·µ»Ø
FLAG_END:
	{
		redisFree(pRC);
	}

	system("pause");
	return 0;
}