#pragma once

typedef void(*TimerProc)(void* pData);
struct ITimer;

ITimer* Schedule(TimerProc oTimerProc, void* pData, int nLoopTime, int nRepeatCount);
void CancelTimer(ITimer* pTimer);
