#include "GameTimer.h"
#include <Windows.h>

double		GameTimer::mSecondsPerCount;
double		GameTimer::mDeltaTime;
__int64	GameTimer::mBaseTime;
__int64	GameTimer::mPausedTime;
__int64	GameTimer::mStopTime;
__int64	GameTimer::mPrevTime;
__int64	GameTimer::mCurrTime;
bool		GameTimer::mStopped;

GameTimer::GameTimer()
{
	mSecondsPerCount = 0.0f;
	mDeltaTime = -1.0f;

	mBaseTime = 0;
	mPausedTime = 0;
	mStopTime = 0;
	mPrevTime = 0;
	mCurrTime = 0;

	mStopped = false;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimer::TotalTime()
{
	//                 |<paused time>|
	//----*-------------*------------*---------*---------*----->time
	//mBaseTime		mStopTime      startTime   mStopTime  mCurrTime
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		//                 |<paused time>|
		//----*-------------*------------*---------*----->time
		//mBaseTime		mStopTime      startTime   mCurrTime

		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float GameTimer::DeltaTime()
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	//				   |<    pause time    >|
	//---*--------------*-------------------*------------->time
	//mBaseTime       mStopTime          startTime

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}