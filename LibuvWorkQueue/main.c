#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

//全局事件循环
static uv_loop_t* g_Loop;

void On_work_cb(uv_work_t* req)
{
    //打印当前线程号
    printf("On_work_cb thread id = %d\n", uv_thread_self());
}

void On_after_work_cb(uv_work_t* req, int status)
{
    //打印当前线程号
    printf("On_after_work_cb thread id = %d\n", uv_thread_self());
    
     free(req);
}

int main(int argc, char** argv)
{
    //创建事件循环
    g_Loop = uv_default_loop();

    //打印当前线程号
    printf("main thread id = %d\n", uv_thread_self());

    //创建一个工作队列请求
    uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));

    //使用工作队列
    uv_queue_work(g_Loop, req, On_work_cb, On_after_work_cb);

    //运行事件循环
    uv_run(g_Loop, UV_RUN_DEFAULT);

    return 0;
}