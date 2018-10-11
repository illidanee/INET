#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "uv.h"

/*
	libuv： 根据注册回调函数的参数决定使用同步还是异步。
		当注册回调函数为NULL时，libuv使用同步方式。
		当注册回调函数不为NULL时，libuv使用异步方式。
*/

/*
	uv_file - 代表文件句柄对象,打开文件以后的文件handle。
	uv_fs_t - 每次请求的结果都是返回result这个值。
		打开文件: result返回打开文件句柄对象uv_file。
		读文件: result读到的数据长度。
		写文件: result为写入的数据长度。
*/

/*
	每个进程在运行的时候,stdin文件句柄与stdout这个文件句柄始终是打开的。
		stdin:标准的输入文件。
		stdout:标准的输出文件。
*/

//全局事件循环
uv_loop_t* g_Loop;
uv_file g_File;

//关闭文件回调函数
void On_fs_close_cb(uv_fs_t* req)
{
	printf("On_fs_close_cb...\n");

	//释放请求资源
	uv_fs_req_cleanup(req);

	//释放内存
	free(req);
}

//读取文件回调函数
void On_fs_read_cb(uv_fs_t* req)
{
	printf("On_fs_read_cb...\n");

	//打印文件内容
	char* pBuffer = (char*)req->data;
	printf("文件内容：%s\n", pBuffer);

	//释放请求资源
	uv_fs_req_cleanup(req);
	memset(req, 0, sizeof(uv_fs_t));

	//关闭文件
	uv_fs_close(g_Loop, req, g_File, On_fs_close_cb);
}

//读取文件回调函数
void On_fs_open_cb(uv_fs_t* req)
{
	printf("On_fs_open_cb...\n");

	//判断是否打开成功
	if (req->result < 0) {
		fprintf(stderr, "async open error: %d\n", (int)req->result);
		assert(0);
	}

	//获取打开文件句柄
	g_File = req->result;

	//释放请求资源
	uv_fs_req_cleanup(req);
	memset(req, 0, sizeof(uv_fs_t));

	//读取文件内容
	char* pBuffer = (char*)malloc(1024);
	memset(pBuffer, 0, 1024);
	req->data = pBuffer;
	uv_buf_t iov = uv_buf_init(pBuffer, 1024);
	uv_fs_read(g_Loop, req, g_File, &iov, 1, 0, On_fs_read_cb);
}

int main()
{
	//创建事件循环
	g_Loop = uv_default_loop();

	//申请内存
	uv_fs_t* req = (uv_fs_t*)malloc(sizeof(uv_fs_t));
	memset(req, 0, sizeof(uv_fs_t));

	//打开文件-异步
	uv_fs_open(g_Loop, req, "./abc.txt", 0, O_RDONLY, On_fs_open_cb);

	//写入标准输出-同步
	uv_fs_t* w_req = (uv_fs_t*)malloc(sizeof(uv_fs_t));
	memset(w_req, 0, sizeof(uv_fs_t));

	uv_buf_t iov = uv_buf_init("hello 123.\n", strlen("hello 123.\n"));
	uv_fs_write(NULL, w_req, (uv_file)1, &iov, 1, 0, NULL);
	uv_fs_req_cleanup(w_req);

	//运行事件循环
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//关闭事件循环
	uv_loop_close(g_Loop);


	system("pause");
	return 0;
}