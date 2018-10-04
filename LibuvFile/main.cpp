#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "uv.h"

//ȫ���¼�ѭ��
uv_loop_t* g_Loop;
uv_file g_File;

//�ر��ļ��ص�����
void On_fs_close_cb(uv_fs_t* req)
{
	printf("On_fs_close_cb...\n");

	//�ͷ�������Դ
	uv_fs_req_cleanup(req);

	//�ͷ��ڴ�
	free(req);
}

//��ȡ�ļ��ص�����
void On_fs_read_cb(uv_fs_t* req)
{
	printf("On_fs_read_cb...\n");

	//��ӡ�ļ�����
	char* pBuffer = (char*)req->data;
	printf("�ļ����ݣ�%s\n", pBuffer);

	//�ͷ�������Դ
	uv_fs_req_cleanup(req);
	memset(req, 0, sizeof(uv_fs_t));

	//�ر��ļ�
	uv_fs_close(g_Loop, req, g_File, On_fs_close_cb);
}

//��ȡ�ļ��ص�����
void On_fs_open_cb(uv_fs_t* req)
{
	printf("On_fs_open_cb...\n");

	//�ж��Ƿ�򿪳ɹ�
	if (req->result < 0) {
		fprintf(stderr, "async open error: %d\n", (int)req->result);
		assert(0);
	}

	//��ȡ���ļ����
	g_File = req->result;

	//�ͷ�������Դ
	uv_fs_req_cleanup(req);
	memset(req, 0, sizeof(uv_fs_t));

	//��ȡ�ļ�����
	char* pBuffer = (char*)malloc(1024);
	memset(pBuffer, 0, 1024);
	req->data = pBuffer;
	uv_buf_t iov = uv_buf_init(pBuffer, 1024);
	uv_fs_read(g_Loop, req, g_File, &iov, 1, 0, On_fs_read_cb);
}

int main()
{
	//�����¼�ѭ��
	g_Loop = uv_default_loop();

	//�����ڴ�
	uv_fs_t* req = (uv_fs_t*)malloc(sizeof(uv_fs_t));
	memset(req, 0, sizeof(uv_fs_t));

	//���ļ�
	uv_fs_open(g_Loop, req, "./abc.txt", O_RDONLY, 0, On_fs_open_cb);

	//�����¼�ѭ��
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//�ر��¼�ѭ��
	uv_loop_close(g_Loop);


	system("pause");
	return 0;
}