#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"
#include "http_parser.h"

typedef void(*WebGetHandle)(uv_stream_t* stream, char* url);
typedef void(*WebPostHandle)(uv_stream_t* stream, char* url, char* body);

struct WebHandleNode {
    char* url;              // url��ַ
    WebGetHandle get;     // url��ַ��Ӧ��get������;
    WebPostHandle post;   // url��ַ��Ӧ��post������
};

static struct WebHandleNode* g_WebHandleNodes[1024];
static int g_nWebHandleNodesCount = 0;

static struct WebHandleNode*
AllocWebHandleNode(char* url, WebGetHandle get, WebPostHandle post) {
    struct WebHandleNode* pNode = (struct WebHandleNode*)malloc(sizeof(struct WebHandleNode));
    memset(pNode, 0, sizeof(struct WebHandleNode));
    pNode->url = strdup(url);
    pNode->get = get;
    pNode->post = post;

    return pNode;
}

static void
RegisterWebHandle(char* url, WebGetHandle get, WebPostHandle post) {
    g_WebHandleNodes[g_nWebHandleNodesCount] = AllocWebHandleNode(url, get, post);
    g_nWebHandleNodesCount++;
}

static struct WebHandleNode*
GetWebHandleNode(char* url, int len) {
    for (int i = 0; i < g_nWebHandleNodesCount; ++i) {
        if (strncmp(url, g_WebHandleNodes[i]->url, len) == 0) {
            return g_WebHandleNodes[i];
        }
    }
    return NULL;
}

//ȫ���¼�ѭ��
uv_loop_t* g_Loop;
//�������
uv_tcp_t* g_Tcp;

void On_write_cb(uv_write_t* req, int status);
void On_close_cb(uv_handle_t* handle);
void On_shutdown_cb(uv_shutdown_t* req, int status);
void On_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void On_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void On_connection_cb(uv_stream_t* server, int status);
void On_connection_cb(uv_stream_t* server, int status);

//д�����ݺ�ص�����
void On_write_cb(uv_write_t* req, int status)
{
    printf("On_write_cb...\n");

    if (req->data) {
        free(req->data);
    }
    free(req);
}

//�رջص�����
void On_close_cb(uv_handle_t* handle)
{
    printf("On_close_cb...\n");

    if (handle->data) {
        free(handle->data);
        handle->data = NULL;
    }

    free(handle);
}

//�Ͽ��ص�����
void On_shutdown_cb(uv_shutdown_t* req, int status)
{
    printf("On_shutdown_cb...\n");

    uv_close((uv_handle_t*)req->handle, On_close_cb);
    free(req);
}

//�����ڴ�ص�����
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

static char g_RequestUrl[4096];

int On_url (http_parser* p, const char *at, size_t length)
{
    strncpy(g_RequestUrl, at, length);
    g_RequestUrl[length] = 0;

    return 0;
}

static int
FilterUrl(char* url) {
    char* walk = url;
    int len = 0;
    while (*url != '?' && *url != '\0') {
        len++;
        url++;
    }

    return len;
}

static void
On_http_request(uv_stream_t* stream, char* req, int len) {
    http_parser_settings settings;
    http_parser_settings_init(&settings);
    settings.on_url = On_url;
    http_parser parser;
    http_parser_init(&parser, HTTP_REQUEST);
    http_parser_execute(&parser, &settings, req, len);

    // http get�ǿ���Я��������:
    // http://www.baidu.com:6080/test?name=blake&age=34
    int nUrlLen = FilterUrl(g_RequestUrl);
    struct WebHandleNode* pNode = GetWebHandleNode(g_RequestUrl, nUrlLen);
    printf("%s\n", g_RequestUrl);

    if (pNode == NULL) {
        return;
    }

    switch (parser.method) { // ���󷽷�
    case HTTP_GET:
        if (pNode->get != NULL) {
            pNode->get(stream, g_RequestUrl);
        }
        break;
    case HTTP_POST:
        if (pNode->post != NULL) {
        }
        break;
    }
}

//��ȡ���ݻص�����
void On_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    printf("On_read_cb...\n");

    // ���ӶϿ���;
    if (nread < 0) {
        uv_shutdown_t* reg = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        memset(reg, 0, sizeof(uv_shutdown_t));
        uv_shutdown(reg, stream, On_shutdown_cb);
        return;
    }

    //��ӡ���յ�������
    buf->base[nread] = 0;
    printf("recv %d\n", nread);
    printf("%s\n", buf->base);

    //����http����
    On_http_request(stream, buf->base, nread);

    ////��������
    //uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    //memset(w_req, 0, sizeof(uv_write_t));
    //uv_buf_t* w_buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
    //memset(w_buf, 0, sizeof(uv_buf_t));
    //w_buf->base = buf->base;
    //w_buf->len = nread;
    //w_req->data = w_buf;
    //uv_write(w_req, stream, w_buf, 1, On_write_cb);
}

//���ӻص�����
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
    //�����¼�ѭ��
    g_Loop = uv_default_loop();

    //��������������󶨡�����
    g_Tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    memset(g_Tcp, 0, sizeof(uv_tcp_t));
    uv_tcp_init(g_Loop, g_Tcp);
    struct sockaddr_in oServerAddr;
    uv_ip4_addr("0.0.0.0", 12345, &oServerAddr);
    uv_tcp_bind(g_Tcp, (const struct sockaddr*)&oServerAddr, 0);

    uv_listen((uv_stream_t*)g_Tcp, SOMAXCONN, On_connection_cb);

    //�����¼�ѭ��
    uv_run(g_Loop, UV_RUN_DEFAULT);

    //�ͷż������
    free(g_Tcp);

    //�ر��¼�ѭ��
    uv_loop_close(g_Loop);

    return 0;
}