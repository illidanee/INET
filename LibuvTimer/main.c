#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

#include "ITimer.h"

//全局事件循环
uv_loop_t* g_Loop;

//定时器句柄
uv_timer_t* g_Timer;

//自定义定时器类
struct ITimer* g_ITimer;

//定时器回调函数
void On_timer_cb(uv_timer_t* handle)
{
	printf("On_timer_cb...\n");
}

//定时器回调函数1
void On_timer_cb1(void* pData)
{
	printf("On_timer_cb1...%s\n", pData);
}

int main()
{
	//创建事件循环
	g_Loop = uv_default_loop();

	//创建定时器并添加定时任务
	g_Timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
	memset(g_Timer, 0, sizeof(uv_timer_t));
	uv_timer_init(g_Loop, g_Timer);

	uv_timer_start(g_Timer, On_timer_cb, 1000, 1000);

	//创建自定义计时器
	g_ITimer = Schedule(On_timer_cb1, "HelloWorld", 1000, 5);

	//运行事件循环
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//销毁自定义计时器
	CancelTimer(g_ITimer);

	//关闭定时器并销毁
	uv_timer_stop(g_Timer);
	free(g_Timer);

	//关闭事件循环
	uv_loop_close(g_Loop);

	return 0;
}