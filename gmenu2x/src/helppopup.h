// Various authors.
// License: GPL version 2 or later.

#ifndef HELPPOPUP_H
#define HELPPOPUP_H

#include "layer.h"

class GMenu2X;


/**
 * A passive dialog containing some helpful text for the user.
 */
class HelpPopup : public Layer {
public:
	HelpPopup(GMenu2X& gmenu2x);

	// Layer implementation:
	virtual void paint(Surface& s);
	virtual bool handleButtonPress(InputManager::Button button);
	virtual bool handleTouchscreen(Touchscreen& ts);

private:
	GMenu2X& gmenu2x;
};

#endif // HELPPOPUP_H
