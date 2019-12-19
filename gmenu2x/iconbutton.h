#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include "gmenu2x.h"

#include <SDL.h>

#include <functional>
#include <string>

class OffscreenSurface;
class Surface;
class Touchscreen;


class IconButton {
public:
	typedef std::function<void(void)> Action;

	IconButton(GMenu2X *gmenu2x, Touchscreen &ts,
			const std::string &icon, const std::string &label = "",
			Action action = nullptr);

	SDL_Rect getRect() { return rect; }
	void setPosition(int x, int y);

	bool handleTS();

	void paint(Surface& s);

private:
	void recalcRects();

	GMenu2X *gmenu2x;
	Touchscreen &ts;
	std::string icon, label;
	Action action;

	SDL_Rect rect, iconRect, labelRect;
	OffscreenSurface *iconSurface;
};

#endif
