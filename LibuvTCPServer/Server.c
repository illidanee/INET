#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

//全局事件循环
uv_loop_t* g_Loop;
//监听句柄
uv_tcp_t* g_Tcp;

void On_write_cb(uv_write_t* req, int status);
void On_close_cb(uv_handle_t* handle);
void On_shutdown_cb(uv_shutdown_t* req, int status);
void On_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void On_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void On_connection_cb(uv_stream_t* server, int status);
void On_connection_cb(uv_stream_t* server, int status);

//写入数据后回调函数
void On_write_cb(uv_write_t* req, int status)
{
	printf("On_write_cb...\n");

	if (req->data) {
		free(req->data);
	}
	free(req);
}

//关闭回调函数
void On_close_cb(uv_handle_t* handle)
{
	printf("On_close_cb...\n");

	if (handle->data) {
		free(handle->data);
		handle->data = NULL;
	}

	free(handle);
}

//断开回调函数
void On_shutdown_cb(uv_shutdown_t* req, int status)
{
	printf("On_shutdown_cb...\n");

	uv_close((uv_handle_t*)req->handle, On_close_cb);
	free(req);
}

//分配内存回调函数
void On_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	printf("On_alloc_cb...\n");

	if (handle->data != NULL) {
		free(handle->data);
		handle->data = NULL;
	}

	char* pBuffer = (char*)malloc(suggested_size + 1);
	memset(pBuffer, 0, suggested_size + 1);
	buf->len = suggested_size;
	buf->base = pBuffer;
	handle->data = pBuffer;
}

//读取数据回调函数
void On_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
	printf("On_read_cb...\n");

	// 连接断开了;
	if (nread < 0) {
		uv_shutdown_t* reg = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
		memset(reg, 0, sizeof(uv_shutdown_t));
		uv_shutdown(reg, stream, On_shutdown_cb);
		return;
	}

	//打印接收到的数据
	buf->base[nread] = 0;
	printf("recv %d\n", nread);
	printf("%s\n", buf->base);

	//发送数据
	uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	memset(w_req, 0, sizeof(uv_write_t));
	uv_buf_t* w_buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	memset(w_buf, 0, sizeof(uv_buf_t));
	w_buf->base = buf->base;
	w_buf->len = nread;
	w_req->data = w_buf;
	uv_write(w_req, stream, w_buf, 1, On_write_cb);
}

//连接回调函数
void On_connection_cb(uv_stream_t* server, int status)
{
	printf("On_Client_Connect...\n");

	uv_tcp_t* pClient = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	memset(pClient, 0, sizeof(uv_tcp_t));
	uv_tcp_init(g_Loop, pClient);
	uv_accept(server, (uv_stream_t*)pClient);
	uv_read_start((uv_stream_t*)pClient, On_alloc_cb, On_read_cb);
}

int main()
{
	//创建事件循环
	g_Loop = uv_default_loop();

	//创建监听句柄并绑定、监听
	g_Tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	memset(g_Tcp, 0, sizeof(uv_tcp_t));
	uv_tcp_init(g_Loop, g_Tcp);
	struct sockaddr_in oServerAddr;
	uv_ip4_addr("0.0.0.0", 12345, &oServerAddr);
	uv_tcp_bind(g_Tcp, (const struct sockaddr*)&oServerAddr, 0);

	uv_listen((uv_stream_t*)g_Tcp, SOMAXCONN, On_connection_cb);

	//运行事件循环
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//释放监听句柄
	free(g_Tcp);

	//关闭事件循环
	uv_loop_close(g_Loop);

	return 0;
}