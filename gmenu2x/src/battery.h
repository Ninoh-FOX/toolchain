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
	OffscreenSurface const& getIcon();

private:
	void update();

	SurfaceCollection& sc;
	std::string iconPath;
	unsigned int lastUpdate;
};

#endif /* __BATTERY_H__ */
