#include "battery.h"

#include "surfacecollection.h"

#include <SDL.h>
#include <cstdio>
#include <sstream>


/**
 * Reads the current battery state and returns a number representing its level
 * of charge.
 * @return A number representing battery charge: 0 means fully discharged,
 * 5 means fully charged, 6 represents running on external power.
 */
static unsigned short getBatteryLevel()
{
	FILE *batteryHandle = NULL, *usbHandle = NULL;

#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0) || defined(PLATFORM_NANONOTE)
	usbHandle = fopen("/sys/class/power_supply/usb/online", "r");
#endif
	if (usbHandle) {
		int usbval = 0;
		fscanf(usbHandle, "%d", &usbval);
		fclose(usbHandle);
		if (usbval == 1)
			return 6;
	}

#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0) || defined(PLATFORM_NANONOTE)
	batteryHandle = fopen("/sys/class/power_supply/battery/capacity", "r");
#endif
	if (batteryHandle) {
		int battval = 0;
		fscanf(batteryHandle, "%d", &battval);
		fclose(batteryHandle);

		if (battval>90) return 5;
		if (battval>70) return 4;
		if (battval>50) return 3;
		if (battval>30) return 2;
		if (battval>10) return 1;
	}

	return 0;
}

Battery::Battery(SurfaceCollection& sc_)
	: sc(sc_)
{
	lastUpdate = SDL_GetTicks();
	update();
}

OffscreenSurface const& Battery::getIcon()
{
	// Check battery status every 60 seconds.
	unsigned int now = SDL_GetTicks();
	if (now - lastUpdate >= 60000) {
		lastUpdate = now;
		update();
	}

	return *sc.skinRes(iconPath);
}

void Battery::update()
{
	unsigned short battlevel = getBatteryLevel();
	if (battlevel > 5) {
		iconPath = "imgs/battery/ac.png";
	} else {
		std::stringstream ss;
		ss << "imgs/battery/" << battlevel << ".png";
		ss >> iconPath;
	}
}
