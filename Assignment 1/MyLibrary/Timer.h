#include <ctime>

class Timer
{
private:
	clock_t startedAt;
	clock_t pausedAt;
	bool started;
	bool paused;
public:
	Timer();
	Timer(float startTime);
	bool IsStarted();
	bool IsStopped();
	bool IsPaused();
	bool IsActive();
	void Pause();
	void Resume();
	void Stop();
	void Start();
	void Reset();
	clock_t GetTicks();
};
