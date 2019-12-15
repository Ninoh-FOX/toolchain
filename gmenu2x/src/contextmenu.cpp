// Various authors.
// License: GPL version 2 or later.

#include "contextmenu.h"

#include "gmenu2x.h"
#include "linkapp.h"
#include "menu.h"
#include "utilities.h"

#include <algorithm>
#include <functional>


struct ContextMenu::MenuOption {
	typedef std::function<void(void)> Action;
	MenuOption(std::string text, Action action)
		: text(text), action(action) {}
	std::string text;
	Action action;
};

ContextMenu::ContextMenu(GMenu2X &gmenu2x, Menu &menu)
	: gmenu2x(gmenu2x)
	, menu(menu)
	, selected(0)
{
	Translator &tr = gmenu2x.tr;
	Font& font = *gmenu2x.font;
	LinkApp* app = menu.selLinkApp();

	// Init menu options:

	options.push_back(std::make_shared<MenuOption>(
			tr.translate("Add link in $1", menu.selSection().c_str(), NULL),
			std::bind(&GMenu2X::addLink, &gmenu2x)));

	if (app) {
		if (!app->getManual().empty()) {
			options.push_back(std::make_shared<MenuOption>(
					tr.translate("Show manual of $1",
						app->getTitle().c_str(), NULL),
					std::bind(&GMenu2X::showManual, &gmenu2x)));
		}

		/* FIXME(percuei): This permits to mask the "Edit link" entry
		 *                 on the contextual menu in case CPUFREQ support is
		 *                 not compiled in and the link corresponds to an OPK.
		 *                 This is not a good idea as it'll break things if
		 *                 a new config option is added to the contextual menu.
		 */
		if (app->isEditable() && (
				!app->isOpk()
#if defined(ENABLE_CPUFREQ)
				|| true
#endif
				|| !app->getSelectorDir().empty()
				)) {
			options.push_back(std::make_shared<MenuOption>(
					tr.translate("Edit $1", app->getTitle().c_str(), NULL),
					std::bind(&GMenu2X::editLink, &gmenu2x)));
		}

		if (app->isDeletable()) {
			options.push_back(std::make_shared<MenuOption>(
					tr.translate("Delete $1", app->getTitle().c_str(), NULL),
					std::bind(&GMenu2X::deleteLink, &gmenu2x)));
		}
	}

	options.push_back(std::make_shared<MenuOption>(
			tr["Add section"],
			std::bind(&GMenu2X::addSection, &gmenu2x)));
	options.push_back(std::make_shared<MenuOption>(
			tr["Rename section"],
			std::bind(&GMenu2X::renameSection, &gmenu2x)));
	options.push_back(std::make_shared<MenuOption>(
			tr["Delete section"],
			std::bind(&GMenu2X::deleteSection, &gmenu2x)));

	// Compute bounding box.
	int w = 0;
	for (auto option : options) {
		w = std::max(w, font.getTextWidth(option->text));
	}
	w += 23;
	const int h = (font.getLineSpacing() + 2) * options.size() + 8;
	box = {
		static_cast<Sint16>((gmenu2x.resX - w) / 2),
		static_cast<Sint16>((gmenu2x.resY - h) / 2),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};

	// Init background fade animation.
	tickStart = SDL_GetTicks();
	fadeAlpha = 0;
}

bool ContextMenu::runAnimations()
{
	if (fadeAlpha < 200) {
		const long tickNow = SDL_GetTicks();
		fadeAlpha = intTransition(0, 200, tickStart, 500, tickNow);
	}
	return fadeAlpha < 200;
}

void ContextMenu::paint(Surface &s)
{
	Font& font = *gmenu2x.font;

	// Darken background.
	s.box(0, 0, gmenu2x.resX, gmenu2x.resY, 0, 0, 0, fadeAlpha);

	// Draw popup box.
	s.box(box, gmenu2x.skinConfColors[COLOR_MESSAGE_BOX_BG]);
	s.rectangle(box.x + 2, box.y + 2, box.w - 4, box.h - 4,
			gmenu2x.skinConfColors[COLOR_MESSAGE_BOX_BORDER]);

	// Draw selection background.
	const int h = font.getLineSpacing();
	SDL_Rect selbox = {
		static_cast<Sint16>(box.x + 4),
		static_cast<Sint16>(box.y + 4 + (h + 2) * selected),
		static_cast<Uint16>(box.w - 8),
		static_cast<Uint16>(h + 2)
	};
	s.box(selbox, gmenu2x.skinConfColors[COLOR_MESSAGE_BOX_SELECTION]);

	// List options.
	for (uint i = 0; i < options.size(); i++) {
		font.write(s, options[i]->text, box.x + 12, box.y + 5 + (h + 2) * i,
				Font::HAlignLeft, Font::VAlignTop);
	}
}

bool ContextMenu::handleButtonPress(InputManager::Button button) {
	switch (button) {
		case InputManager::MENU:
			dismiss();
			break;
		case InputManager::UP:
			selected--;
			if (selected < 0) selected = options.size() - 1;
			break;
		case InputManager::DOWN:
			selected++;
			if (selected >= static_cast<int>(options.size())) selected = 0;
			break;
		case InputManager::ACCEPT:
			options[selected]->action();
			dismiss();
			break;
		default:
			break;
	}
	return true;
}

bool ContextMenu::handleTouchscreen(Touchscreen &ts) {
	if (ts.inRect(box)) {
		int i = std::max(0, std::min(static_cast<int>(options.size()) - 1,
				(ts.getY() - (box.y + 4)) / (gmenu2x.font->getLineSpacing() + 2)));
		if (ts.released()) {
			options[i]->action();
			dismiss();
		} else if (ts.pressed()) {
			selected = i;
		}
	} else {
		if (ts.released()) {
			dismiss();
		}
	}
	return true;
}
