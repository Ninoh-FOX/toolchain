// Various authors.
// License: GPL version 2 or later.

#ifndef __CONTEXTMENU_H__
#define __CONTEXTMENU_H__

#include "layer.h"

#include <SDL.h>
#include <memory>

class GMenu2X;
class Menu;


/**
 * A popup dialog containing action on the current section or link.
 */
class ContextMenu : public Layer {
public:
	ContextMenu(GMenu2X &gmenu2x, Menu &menu);

	// Layer implementation:
	virtual bool runAnimations();
	virtual void paint(Surface &s);
	virtual bool handleButtonPress(InputManager::Button button);
	virtual bool handleTouchscreen(Touchscreen &ts);

private:
	struct MenuOption;

	GMenu2X &gmenu2x;
	Menu &menu;
	std::vector<std::shared_ptr<MenuOption>> options;
	SDL_Rect box;

	int fadeAlpha;
	int selected;
	long tickStart;
};

#endif // __CONTEXTMENU_H__
