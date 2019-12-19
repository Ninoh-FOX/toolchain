/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo                           *
 *   massimiliano.torromeo@gmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "messagebox.h"
#include "gmenu2x.h"
#include "surface.h"

#include <unistd.h>

using namespace std;

constexpr unsigned int ICON_PADDING = 6;
constexpr unsigned int TEXT_PADDING = 8;
constexpr unsigned int ICON_DIMENSION = 32;

MessageBox::MessageBox(GMenu2X *gmenu2x, const string &text, const string &icon) {
	this->gmenu2x = gmenu2x;
	this->text = text;
	this->icon = icon;

	for (uint i = 0; i < BUTTON_TYPE_SIZE; i++) {
		buttons[i] = "";
		buttonLabels[i] = "";
		buttonPositions[i].h = gmenu2x->font->getLineSpacing();
	}

	//Default enabled button
	buttons[InputManager::ACCEPT] = "OK";

	//Default labels
	buttonLabels[InputManager::UP] = "up";
	buttonLabels[InputManager::DOWN] = "down";
	buttonLabels[InputManager::LEFT] = "left";
	buttonLabels[InputManager::RIGHT] = "right";
	buttonLabels[InputManager::CANCEL] = "cancel";
	buttonLabels[InputManager::ACCEPT] = "accept";
	buttonLabels[InputManager::ALTLEFT] = "l";
	buttonLabels[InputManager::ALTRIGHT] = "r";
	buttonLabels[InputManager::SETTINGS] = "start";
	buttonLabels[InputManager::MENU] = "select";
}

void MessageBox::setButton(InputManager::Button button, const string &label) {
	buttons[button] = label;
}

int MessageBox::exec() {
	OutputSurface& s = *gmenu2x->s;
	OffscreenSurface bg(s);
	//Darken background
	bg.box(0, 0, gmenu2x->resX, gmenu2x->resY, 0,0,0,200);

	SDL_Rect box;
	int textHeight = gmenu2x->font->getTextHeight(text);
	box.h = textHeight + 2 * TEXT_PADDING;
	box.w = gmenu2x->font->getTextWidth(text) + 2 * TEXT_PADDING;
	if (gmenu2x->sc[icon]) {
		box.h = max(box.h, (Uint16) (ICON_DIMENSION + 2 * ICON_PADDING));
		box.w += ICON_DIMENSION + ICON_PADDING;
	}
	box.x = gmenu2x->halfX - box.w/2;
	box.y = gmenu2x->halfY - box.h/2;

	//outer box
	bg.box(box.x - 2, box.y - 2, box.w + 4, box.h + 4, gmenu2x->skinConfColors[COLOR_MESSAGE_BOX_BG]);
	//draw inner rectangle
	bg.rectangle(box, gmenu2x->skinConfColors[COLOR_MESSAGE_BOX_BORDER]);
	//icon+text
	if (gmenu2x->sc[icon]) {
		gmenu2x->sc[icon]->blitCenter(bg, box.x + ICON_PADDING + ICON_DIMENSION / 2, box.y + ICON_PADDING + ICON_DIMENSION / 2);
	}
	gmenu2x->font->write(bg, text, box.x + TEXT_PADDING + (gmenu2x->sc[icon] ? ICON_PADDING + ICON_DIMENSION : 0), box.y + (box.h - textHeight) / 2, Font::HAlignLeft, Font::VAlignTop);

	int btnX = gmenu2x->halfX+box.w/2-6;
	for (uint i = 0; i < BUTTON_TYPE_SIZE; i++) {
		if (!buttons[i].empty()) {
			buttonPositions[i].y = box.y+box.h+8;
			buttonPositions[i].w = btnX;

			btnX = gmenu2x->drawButtonRight(bg, buttonLabels[i], buttons[i], btnX, buttonPositions[i].y);

			buttonPositions[i].x = btnX;
			buttonPositions[i].w = buttonPositions[i].x-btnX-6;
		}
	}

	bg.convertToDisplayFormat();
	bg.blit(s, 0, 0);
	s.flip();

	int result = -1;
	while (result < 0) {
		InputManager::Button button;
		if (gmenu2x->input.pollButton(&button)
				&& !buttons[button].empty()) {
			result = button;
		}

		usleep(LOOP_DELAY);
	}

	return result;
}
