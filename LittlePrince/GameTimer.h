#pragma once
class GameTimer
{
public:
	GameTimer();

	static float TotalTime();//unit is second
	static float DeltaTime();//unit is second

	//call before message loop
	static void Reset();
	//cancel timer stop
	static void Start();
	//timer stop
	static void Stop();
	//called once per frame
	static void Tick();

private:
	static double mSecondsPerCount;
	static double mDeltaTime;

	static __int64 mBaseTime;
	static __int64 mPausedTime;
	static __int64 mStopTime;
	static __int64 mPrevTime;
	static __int64 mCurrTime;

	static bool mStopped;
};
