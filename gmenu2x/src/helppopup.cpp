// Various authors.
// License: GPL version 2 or later.

#include "helppopup.h"

#include "gmenu2x.h"


HelpPopup::HelpPopup(GMenu2X& gmenu2x)
	: gmenu2x(gmenu2x)
{
}

void HelpPopup::paint(Surface& s) {
	Font& font = *gmenu2x.font;
	Translator &tr = gmenu2x.tr;
	int helpBoxHeight = 154;

	s.box(10, 50, 300, helpBoxHeight + 4,
			gmenu2x.skinConfColors[COLOR_MESSAGE_BOX_BG]);
	s.rectangle(12, 52, 296, helpBoxHeight,
			gmenu2x.skinConfColors[COLOR_MESSAGE_BOX_BORDER]);
	font.write(s, tr["CONTROLS"], 20, 60);
#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0)
	font.write(s, tr["A: Launch link / Confirm action"], 20, 80);
	font.write(s, tr["B: Show this help menu"], 20, 95);
	font.write(s, tr["L, R: Change section"], 20, 110);
	font.write(s, tr["SELECT: Show contextual menu"], 20, 155);
	font.write(s, tr["START: Show options menu"], 20, 170);
#endif
}

bool HelpPopup::handleButtonPress(InputManager::Button button) {
	if (button == InputManager::CANCEL) {
		dismiss();
	}
	return true;
}

bool HelpPopup::handleTouchscreen(Touchscreen& ts) {
	if (ts.pressed()) {
		dismiss();
		ts.setHandled();
	}
	return true;
}
