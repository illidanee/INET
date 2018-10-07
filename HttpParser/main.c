#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "http_parser.h"

static char* pHttpGetReq = 
	"GET /favicon.ico HTTP/1.1\r\n"
	"Host: 0.0.0.0=5000\r\n"
	"User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	"Accept-Language: en-us,en;q=0.5\r\n"
	"Accept-Encoding: gzip,deflate\r\n"
	"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
	"Keep-Alive: 300\r\n"
	"Connection: keep-alive\r\n"
	"\r\n";

static char * pHttpPostReq =
	"POST /post_identity_body_world?q=search#hey HTTP/1.1\r\n"
	"Accept: */*\r\n"
	"Transfer-Encoding: identity\r\n"
	"Content-Length: 15\r\n"
	"\r\n"
	"Hello World!!!!";

static char* pHttpGetRes =
	"HTTP/1.1 200 OK\r\n"
	"Date: Sat, 31 Dec 2005 23:59:59 GMT\r\n"
	"Content-Type: text/html;charset=ISO-8859-1\r\n"
	"Content-Length: 122\r\n"
	"\r\n"
	"<html>\r\n"
	"<head>\r\n"
	"<title>Wrox Homepage</title>\r\n"
	"</head>\r\n"
	"<body>\r\n"
	"<!--body goes here -->\r\n"
	"</body>\r\n"
	"</html>\r\n";

static
int On_http_message_begin_cb (http_parser* p)
{
	printf("-- On_http_message_begin_cb\n");
	return 0;
}

static
int On_http_url_cb (http_parser* p, const char *at, size_t length)
{
	static char url_buf[1024];
	strncpy(url_buf, at, length);
	url_buf[length] = 0;
	printf("%s\n", url_buf);

	return 0;
}

static
int On_http_status_cb(http_parser* p, const char *at, size_t length)
{
	static char status[1024];
	strncpy(status, at, length);
	status[length] = 0;
	printf("status: %s\n", status);

	return 0;
}

static
int On_http_header_field_cb(http_parser* p, const char *at, size_t length)
{
	static char filed[1024];
	strncpy(filed, at, length);
	filed[length] = 0;
	printf("%s\n", filed);

	return 0;
}

static
int On_http_header_value_cb(http_parser* p, const char *at, size_t length)
{
	static char value[1024];
	strncpy(value, at, length);
	value[length] = 0;
	printf("%s\n", value);

	return 0;
}

static
int On_http_headers_complete_cb(http_parser* p)
{
	printf("-- On_http_headers_complete_cb\n");

	return 0;
}

static
int On_http_body_cb(http_parser* p, const char *at, size_t length)
{
	static char body[4096];
	strncpy(body, at, length);
	body[length] = 0;
	printf("body: %s\n", body);

	return 0;
}

static
int On_http_message_complete_cb(http_parser* p)
{
	printf("-- On_http_message_complete_cb\n");

	return 0;
}

static
http_parser_settings settings = {
	.on_message_begin		= On_http_message_begin_cb,
	.on_url					= On_http_url_cb,
	.on_status				= On_http_status_cb,
	.on_header_field		= On_http_header_field_cb,
	.on_header_value		= On_http_header_value_cb,
	.on_headers_complete	= On_http_headers_complete_cb,
	.on_body				= On_http_body_cb,
	.on_message_complete	= On_http_message_complete_cb
};

static http_parser ReqParser;		//请求解析器
static http_parser ResParser;		//响应解析器

int main()
{
//解析请求
	http_parser_init(&ReqParser, HTTP_REQUEST);

	printf("-------------------------------------------------------------\n");
	http_parser_execute(&ReqParser, &settings, pHttpGetReq, strlen(pHttpGetReq));

	//printf("-------------------------------------------------------------\n");
	//http_parser_execute(&ReqParser, &settings, pHttpPostReq, strlen(pHttpPostReq));

//解析响应
	http_parser_init(&ResParser, HTTP_RESPONSE);

	printf("-------------------------------------------------------------\n");
	http_parser_execute(&ResParser, &settings, pHttpGetRes, strlen(pHttpGetRes));

	system("pause");
	return 0;
} 