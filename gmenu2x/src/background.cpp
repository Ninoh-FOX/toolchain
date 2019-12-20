// Various authors.
// License: GPL version 2 or later.

#include "background.h"

#include "gmenu2x.h"


Background::Background(GMenu2X& gmenu2x)
	: gmenu2x(gmenu2x)
	, battery(gmenu2x.sc)
{
}

void Background::paint(Surface& s) {
	Font& font = *gmenu2x.font;
	OffscreenSurface& bgmain = *gmenu2x.bgmain;

	bgmain.blit(s, 0, 0);

	font.write(s, clock.getTime(),
			s.width() - 70, gmenu2x.bottomBarTextY,
			Font::HAlignCenter, Font::VAlignMiddle);

	battery.getIcon()->blit(s, s.width() - 19, gmenu2x.bottomBarIconY);
		
	// rafavico, fix por Ninoh-fox -->
	font.write(s, battery.getLevel(),
			s.width() - 34, gmenu2x.bottomBarTextY,
			Font::HAlignCenter, Font::VAlignMiddle);
    // <-- rafavico

}

bool Background::handleButtonPress(InputManager::Button button) {
	switch (button) {
		case InputManager::CANCEL:
			gmenu2x.showHelpPopup();
			return true;
		case InputManager::SETTINGS:
			gmenu2x.showSettings();
			return true;
		default:
			return false;
	}
}

bool Background::handleTouchscreen(Touchscreen&) {
	return false;
}
