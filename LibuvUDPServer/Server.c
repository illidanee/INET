#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

//全局事件循环
uv_loop_t* g_Loop;
//监听句柄
uv_udp_t* g_Udp;

void On_udp_send_cb(uv_udp_send_t* req, int status)
{
	printf("On_udp_send_cb...\n");

	free(req);
}

void On_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	printf("On_alloc_cb...\n");

	if (handle->data != NULL)
	{
		free(handle->data);
		handle->data = NULL;
	}

	char* pBuffer = (char*)malloc(suggested_size + 1);
	memset(pBuffer, 0, suggested_size + 1);
	buf->len = suggested_size;
	buf->base = pBuffer;
	handle->data = pBuffer;
}

void On_udp_recv_cb(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	printf("On_udp_recv_cb...\n");

	//打印接收到的数据
	buf->base[nread] = 0;
	printf("IP : %s, Port : %d， recv %d\n", inet_ntoa(((struct sockaddr_in*)addr)->sin_addr), ntohs(((struct sockaddr_in*)addr)->sin_port), nread);
	printf("%s\n", buf->base);

	//发送数据
	uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
	memset(req, 0, sizeof(uv_udp_send_t));
	uv_buf_t w_buf = uv_buf_init(buf->base, nread);
	uv_udp_send(req, handle, &w_buf, 1, addr, On_udp_send_cb);
}

int main()
{
	//创建事件循环
	g_Loop = uv_default_loop();

	//创建监听句柄并绑定、监听
	g_Udp = (uv_udp_t*)malloc(sizeof(uv_udp_t));
	memset(g_Udp, 0, sizeof(uv_udp_t));
	uv_udp_init(g_Loop, g_Udp);
	struct sockaddr_in oServerAddr;
	uv_ip4_addr("0.0.0.0", 12345, &oServerAddr);
	uv_udp_bind(g_Udp, (const struct sockaddr*)&oServerAddr, 0);

	uv_udp_recv_start(g_Udp, On_alloc_cb, On_udp_recv_cb);

	//运行事件循环
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//释放监听句柄
	free(g_Udp);

	//关闭事件循环
	uv_loop_close(g_Loop);

	return 0;
}