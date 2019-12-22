#ifndef POWERSAVER_H
#define POWERSAVER_H

#include <SDL.h>

class PowerSaver {
public:
	PowerSaver();
	~PowerSaver();
	void resetScreenTimer();
	void setScreenTimeout(unsigned int seconds);
	bool getState();
	void enablePowerSaver();

private:
	void addScreenTimer();
	void removeScreenTimer();
	void setScreenBlanking(bool state);
	void enableScreen();
	void disableScreen();

	static PowerSaver *instance;
	bool screenState;
	unsigned int screenTimeout;
	unsigned int timeout_startms;
	SDL_TimerID screenTimer;

	friend Uint32 screenTimerCallback(Uint32 timeout, void *d);
};

#endif
