#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();
	float TotalTime() const;//compute the total time from begin,except the pause
	float DeltaTime()  const;//return the delta time from last tick to current tick

	void Reset();//reset the tick
	void Start();//begin tick after pause
	void Stop();//pause
	void Tick();//tick once,call every frame

private:
	double m_secondsPerCount;//the delta time of system's two count,used to convert the count to time
	double m_deltaTime;//the delta time since last tick

	__int64 m_baseTime;//the moment of the timer starting tick
	__int64 m_pausedTime;//the total pause time
	__int64 m_stopTime;//the begin of the pause
	__int64 m_prevTime;//the time of last tick
	__int64 m_currTime;//the time of current tick

	bool m_isStopped;//whether stop
};

#endif