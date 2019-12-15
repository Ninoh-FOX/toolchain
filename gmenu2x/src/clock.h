#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <memory>
#include <string>


/**
 * A keeper of wall clock time with minute accuracy.
 * Sends a user event on every minute boundary, to force a repaint of the
 * clock display.
 */
class Clock {
public:
	/**
	 * Used by implementation; please ignore.
	 */
	class Timer;

	Clock();

	/**
	 * Gets a string representation of the current time.
	 * Uses 24-hour format if is24 is true, otherwise AM/PM.
	 */
	std::string getTime(bool is24 = true);

private:
	std::shared_ptr<Timer> timer;
};

#endif /* __CLOCK_H__ */
