#ifndef TIMER_H
#define TIMER_H

enum timer_type
{
	TIMER_COUNTDOWN,
	TIMER_DEFAULT
};

struct timer
{
	real32 Time;
	real32 EndTime;
	real32 DefaultEndTime;
	bool Active;
	bool Complete;
	timer_type Type;
};

real32 Win32GetSecondElapsed(uint64 LastCount, uint64 CurrentCount);

timer CreateTimer(real32 EndTime, bool Active = true, timer_type Type = TIMER_DEFAULT)
{
	timer Timer = {};
	switch(Type)
	{
		case TIMER_DEFAULT:
		{
			Timer.EndTime = EndTime;
			Timer.DefaultEndTime = EndTime;
			Timer.Active = Active;
			Timer.Complete = false;
		} break;
		case TIMER_COUNTDOWN:
		{
			Timer.DefaultEndTime = EndTime;
			Timer.Time = EndTime;
			Timer.Active = Active;
			Timer.Complete = false;
		} break;
	}
	
	Timer.Type = Type;
	
	return Timer;
}

void StartTimer(timer* Timer)
{
	Timer->Complete = false;
	Timer->Active = true;
}

void PauseTimer(timer* Timer, uint64 LastCount)
{
	Timer->Active = false;
	real32 TimeElapsed = Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
	Timer->Time += TimeElapsed;
}

void StopTimer(timer* Timer)
{
	Timer->Active = false;
}

void ResetTimer(timer* Timer)
{
	Timer->Time = 0;
	//printf("Timer Reset\n");
}

void ResetAndStopTimer(timer* Timer, uint64 LastCount)
{
	ResetTimer(Timer);
	Timer->Active = false;
}

void PrintTime(timer* Timer)
{
	printf("%.1f\n", Timer->Time);
}

void DecreaseEndtime(timer* Timer, real32 DecreaseFactor, real32 LowestEndTime)
{
	if(Timer->Complete)
	{
		if(Timer->EndTime > LowestEndTime)
		{
			Timer->EndTime -= (real32)pow(2, (double)DecreaseFactor);
		}
		else if(Timer->EndTime < LowestEndTime)
		{
			Timer->EndTime = LowestEndTime;
		}
	}
}

void SetEndtimeToDefault(timer* Timer)
{
	Timer->EndTime = Timer->DefaultEndTime;
}

void SetTimeToDefault(timer* Timer)
{
	Timer->Time = Timer->DefaultEndTime;
}

void ProcessTimer(timer* Timer, real32 ElapsedTime)
{
	switch(Timer->Type)
	{
		case TIMER_DEFAULT:
		{
			Timer->Complete = false;
			if(Timer->Active)
			{
				Timer->Time += ElapsedTime;
				if(Timer->Time >= Timer->EndTime && Timer->EndTime > 0)
				{
					Timer->Complete = true;
					//printf("Time to complete: %.1f\n", Timer->Time);
					ResetTimer(Timer);
				}
				//PrintTime(Timer);
			}
		} break;
		case TIMER_COUNTDOWN:
		{
			if(Timer->Active)
			{
				Timer->Time -= ElapsedTime;
				if(Timer->Time <= Timer->EndTime)
				{
					Timer->Complete = true;
					Timer->Active = false;
					SetTimeToDefault(Timer);
				}
			}
		} break;
	}
	
}

real32 Win32GetSecondElapsed(uint64 LastCount, uint64 CurrentCount)
{
	real32 Result = (real32)((CurrentCount - LastCount) * SecPerCount);
	return Result;
}

#endif