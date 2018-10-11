#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

#include "ITimer.h"

//ȫ���¼�ѭ��
uv_loop_t* g_Loop;

//��ʱ�����
uv_timer_t* g_Timer;

//�Զ��嶨ʱ����
struct ITimer* g_ITimer;

//��ʱ���ص�����
void On_timer_cb(uv_timer_t* handle)
{
	printf("On_timer_cb...\n");
}

//��ʱ���ص�����1
void On_timer_cb1(void* pData)
{
	printf("On_timer_cb1...%s\n", pData);
}

int main()
{
	//�����¼�ѭ��
	g_Loop = uv_default_loop();

	//������ʱ������Ӷ�ʱ����
	g_Timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
	memset(g_Timer, 0, sizeof(uv_timer_t));
	uv_timer_init(g_Loop, g_Timer);

	uv_timer_start(g_Timer, On_timer_cb, 1000, 1000);

	//�����Զ����ʱ��
	g_ITimer = Schedule(On_timer_cb1, "HelloWorld", 1000, 5);

	//�����¼�ѭ��
	uv_run(g_Loop, UV_RUN_DEFAULT);

	//�����Զ����ʱ��
	CancelTimer(g_ITimer);

	//�رն�ʱ��������
	uv_timer_stop(g_Timer);
	free(g_Timer);

	//�ر��¼�ѭ��
	uv_loop_close(g_Loop);

	return 0;
}