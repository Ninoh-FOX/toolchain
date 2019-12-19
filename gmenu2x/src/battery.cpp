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
// la función de lectura de batería se divide en 2, una para ver carga y otra para ver si está conectada por usb
static unsigned short getBatteryLevel()
{
	FILE *batteryHandle = NULL;

#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0) || defined(PLATFORM_NANONOTE)
	batteryHandle = fopen("/sys/class/power_supply/battery/capacity", "r");
#endif
	if (batteryHandle) {
		int battval = 0;
		fscanf(batteryHandle, "%d", &battval);
		fclose(batteryHandle);

		if(battval>100)
      battval=100;
		return battval;
	}

	return 0;
}

static unsigned short isBatteryCharging()
{
	FILE *usbHandle = NULL;

#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0) || defined(PLATFORM_NANONOTE)
	usbHandle = fopen("/sys/class/power_supply/usb/online", "r");
#endif
	if (usbHandle) {
		int usbval = 0;
		fscanf(usbHandle, "%d", &usbval);
		fclose(usbHandle);

		return usbval;
	}

	return 0;
}

Battery::Battery(SurfaceCollection& sc_)
	: sc(sc_)
{
	lastUpdate = SDL_GetTicks();
	update();
}

const OffscreenSurface *Battery::getIcon()
{
	// Check battery status every 60 seconds.
	unsigned int now = SDL_GetTicks();
	if (now - lastUpdate >= 60000) {
		lastUpdate = now;
		update();
	}

	return sc.skinRes(iconPath);
}

std::string Battery::getLevel()
{
  std::stringstream lv;
  lv << getBatteryLevel() << "%";  // no aparece el porcentaje (fix ninoh-fox)
  std::string value;
  lv >> value;
  return value;
}

void Battery::update()
{
	unsigned short battlevel = getBatteryLevel();
	if (isBatteryCharging()) {
		iconPath = "imgs/battery/ac.png";
	} else {
    int iconid=1+battlevel/20;
    if(iconid>5)
      iconid=5;
		std::stringstream ss;
		ss << "imgs/battery/" << iconid << ".png";   // rafa vico
		ss >> iconPath;
	}
}
