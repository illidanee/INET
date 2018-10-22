#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

//ȫ���¼�ѭ��
static uv_loop_t* g_Loop;

void On_work_cb(uv_work_t* req)
{
    //��ӡ��ǰ�̺߳�
    printf("On_work_cb thread id = %d\n", uv_thread_self());
}

void On_after_work_cb(uv_work_t* req, int status)
{
    //��ӡ��ǰ�̺߳�
    printf("On_after_work_cb thread id = %d\n", uv_thread_self());
    
     free(req);
}

int main(int argc, char** argv)
{
    //�����¼�ѭ��
    g_Loop = uv_default_loop();

    //��ӡ��ǰ�̺߳�
    printf("main thread id = %d\n", uv_thread_self());

    //����һ��������������
    uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));

    //ʹ�ù�������
    uv_queue_work(g_Loop, req, On_work_cb, On_after_work_cb);

    //�����¼�ѭ��
    uv_run(g_Loop, UV_RUN_DEFAULT);

    return 0;
}