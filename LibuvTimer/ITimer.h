#ifndef __ITIMER_H__
#define __ITIMER_H__

typedef void(*TimerProc)(void* pData);
struct ITimer;

struct ITimer* Schedule(TimerProc oTimerProc, void* pData, int nLoopTime, int nRepeatCount);
void CancelTimer(struct ITimer* pTimer);

#endif