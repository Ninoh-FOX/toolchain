#include "iconbutton.h"

#include "font.h"
#include "gmenu2x.h"
#include "surface.h"

using namespace std;


IconButton::IconButton(
		GMenu2X *gmenu2x, Touchscreen &ts,
		const string &icon, const string &label, Action action)
	: gmenu2x(gmenu2x)
	, ts(ts)
	, icon(icon)
	, label(label)
	, action(action)
	, rect({ 0, 0, 0, 0 })
{
	iconSurface = gmenu2x->sc[icon];
	recalcRects();
}

void IconButton::setPosition(int x, int y) {
	if (rect.x != x || rect.y != y) {
		rect.x = x;
		rect.y = y;
		recalcRects();
	}
}

void IconButton::recalcRects() {
	Uint16 h = 0, w = 0;
	if (iconSurface) {
		w += iconSurface->width();
		h += iconSurface->height();
	}
	iconRect = { rect.x, rect.y, w, h };

	if (!label.empty()) {
		Uint16 margin = iconSurface ? 2 : 0;
		labelRect = {
			static_cast<Sint16>(iconRect.x + iconRect.w + margin),
			static_cast<Sint16>(rect.y + h / 2),
			static_cast<Uint16>(gmenu2x->font->getTextWidth(label)),
			static_cast<Uint16>(gmenu2x->font->getLineSpacing())
		};
		w += margin + labelRect.w;
	}

	rect.w = w;
	rect.h = h;
}

bool IconButton::handleTS() {
	if (action && ts.released() && ts.inRect(rect)) {
		ts.setHandled();
		action();
		return true;
	}
	return false;
}

void IconButton::paint(Surface& s) {
	if (iconSurface) {
		iconSurface->blit(s, iconRect);
	}
	if (!label.empty()) {
		gmenu2x->font->write(s, label, labelRect.x, labelRect.y,
				Font::HAlignLeft, Font::VAlignMiddle);
	}
}
