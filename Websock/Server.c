#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"
#include "http_parser.h"
#include "sha1.h"
#include "base64_encoder.h"

struct WSContex
{
	int isHand;
	char* data;
};

//全局事件循环
uv_loop_t* g_Loop;
//监听句柄
uv_tcp_t* g_Tcp;
//http parser
http_parser_settings g_ParserSettings;
http_parser g_Parser;
static char field_sec_key[256];
static char value_sec_key[256];
static int g_IsKey = 0;
static int g_HasKey = 0;

static char* wb_migic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static char *wb_accept = "HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade:websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: %s\r\n"
"WebSocket-Protocol:chat\r\n\r\n";


void On_write_cb(uv_write_t* req, int status);
void On_close_cb(uv_handle_t* handle);
void On_shutdown_cb(uv_shutdown_t* req, int status);
void On_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void On_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void On_connection_cb(uv_stream_t* server, int status);
void On_connection_cb(uv_stream_t* server, int status);

static void
SendData(uv_stream_t* stream, char* pSendData, int nSendLen);

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
        struct WSContex* pWC = (struct WSContex*)handle->data;
		free(pWC->data);
		pWC->data = NULL;
		free(pWC);
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
    struct WSContex* pWC = (struct WSContex*)handle->data;

	if (pWC->data != NULL) {
		free(pWC->data);
		pWC->data = NULL;
	}

	char* pBuffer = (char*)malloc(suggested_size + 1);
	memset(pBuffer, 0, suggested_size + 1);
	buf->len = suggested_size;
	buf->base = pBuffer;
	pWC->data = pBuffer;
}

static void
ShakeHand(uv_stream_t* stream, char* pSendData, int nSendLen)
{
	g_IsKey = 0;
	g_HasKey = 0;
	http_parser_execute(&g_Parser, &g_ParserSettings, pSendData, nSendLen);

	if (g_HasKey)
	{
		printf("Sec-WebSocket-Key : %s \n", value_sec_key);
		
		//int nSha1Len;
		//char key_migic[512] = {};
		//char sha1_key_migic[SHA1_DIGEST_SIZE] = {};
		//sprintf(key_migic, "%s%s", value_sec_key, wb_migic);
		//crypt_sha1((uint8_t*)key_migic, strlen(key_migic), (uint8_t*)&sha1_key_migic, &nSha1Len);

		//int nEncodeLen;
		//char* base64_buff = base64_encode((uint8_t*)sha1_key_migic, nSha1Len, &nEncodeLen);
		//char clientRes[1024] = {};
		//sprintf(clientRes, wb_accept, base64_buff);
		//base64_encode_free(base64_buff);

		//SendData(stream, clientRes, strlen(clientRes));
	}
}

static void
SendData(uv_stream_t* stream, char* pSendData, int nSendLen)
{
	uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	memset(w_req, 0, sizeof(uv_write_t));
	uv_buf_t* w_buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	memset(w_buf, 0, sizeof(uv_buf_t));
	w_buf->base = pSendData;
	w_buf->len = nSendLen;
	w_req->data = w_buf;
	uv_write(w_req, stream, w_buf, 1, On_write_cb);
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
	//buf->base[nread] = 0;
	//printf("recv %d\n", nread);
	//printf("%s\n", buf->base);
	printf("Start WebSock...\n");
	
    struct WSContex* pWC = (struct WSContex*)stream->data;

	if (!pWC->isHand)
	{
		ShakeHand(stream, buf->base, buf->len);
		pWC->isHand = 1;
		return;
	}

	//发送数据
	SendData(stream, buf->base, buf->len);
}

//连接回调函数
void On_connection_cb(uv_stream_t* server, int status)
{
	printf("On_Client_Connect...\n");

	uv_tcp_t* pClient = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	memset(pClient, 0, sizeof(uv_tcp_t));
	uv_tcp_init(g_Loop, pClient);
	uv_accept(server, (uv_stream_t*)pClient);

    struct WSContex* pWC = (struct WSContex*)malloc(sizeof(struct WSContex));
	memset(pWC, 0, sizeof(struct WSContex));

	pClient->data = pWC;

	uv_read_start((uv_stream_t*)pClient, On_alloc_cb, On_read_cb);
}

static int
on_ws_header_field(http_parser* p, const char *at, size_t length)
{
	if (strncmp(at, "Sec-WebSocket-Key", length) == 0)
	{
		g_IsKey = 1;
	}
	else
	{
		g_IsKey = 0;
	}
	
	return 0;
}

static int 
on_ws_value_field(http_parser* p, const char *at, size_t length)
{
	if (!g_IsKey)
	{
		return 0;
	}

	strncpy(value_sec_key, at, length);
	value_sec_key[length] = 0;
	g_HasKey = 1;

	return 0;
}

int main()
{
	http_parser_settings_init(&g_ParserSettings);
	g_ParserSettings.on_header_field = on_ws_header_field;
	g_ParserSettings.on_header_value = on_ws_value_field;

	http_parser_init(&g_Parser, HTTP_REQUEST);

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