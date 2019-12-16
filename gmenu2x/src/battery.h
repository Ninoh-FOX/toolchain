#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <string>

class OffscreenSurface;
class SurfaceCollection;


/**
 * Keeps track of the battery status.
 */
class Battery {
public:
	Battery(SurfaceCollection& sc);

	/**
	 * Gets the icon that reflects the current battery status.
	 */
	const OffscreenSurface *getIcon();
	std::string getLevel();       // <-- rafa vico

private:
	void update();

	SurfaceCollection& sc;
	std::string iconPath;
	std::string lv;
	unsigned int lastUpdate;
};

#endif /* __BATTERY_H__ */
