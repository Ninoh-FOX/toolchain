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

#include "inputdialog.h"

#include "buttonbox.h"
#include "gmenu2x.h"
#include "iconbutton.h"
#include "surface.h"
#include "utilities.h"

using namespace std;

#define KEY_WIDTH 20
#define KEY_HEIGHT 20
#define KB_TOP 90

static bool utf8Code(unsigned char c)
{
	return (c>=194 && c<=198) || c==208 || c==209;
}

InputDialog::InputDialog(GMenu2X *gmenu2x, InputManager &inputMgr_,
		Touchscreen &ts_, const string &text,
		const string &startvalue, const string &title, const string &icon)
	: Dialog(gmenu2x)
	, inputMgr(inputMgr_)
	, ts(ts_)
{
	if (title.empty()) {
		this->title = text;
		this->text = "";
	} else {
		this->title = title;
		this->text = text;
	}
	this->icon = "";
	if (!icon.empty() && gmenu2x->sc[icon] != NULL) {
		this->icon = icon;
	}

	input = startvalue;
	selCol = 0;
	selRow = 0;
	keyboard.resize(7);

	keyboard[0].push_back("abcdefghijklm");
	keyboard[0].push_back("nopqrstuvwxyz");
	keyboard[0].push_back("0123456789.  ");

	keyboard[1].push_back("ABCDEFGHIJKLM");
	keyboard[1].push_back("NOPQRSTUVWXYZ");
	keyboard[1].push_back("_\"'`.,:;!?   ");


	keyboard[2].push_back("¡¿*+-/\\&<=>|");
	keyboard[2].push_back("()[]{}@#$%^~");
	keyboard[2].push_back("_\"'`.,:;!?  ");


	keyboard[3].push_back("àáèéìíòóùúýäõ");
	keyboard[3].push_back("ëïöüÿâêîôûåãñ");
	keyboard[3].push_back("čďěľĺňôřŕšťůž");
	keyboard[3].push_back("ąćęłńśżź     ");

	keyboard[4].push_back("ÀÁÈÉÌÍÒÓÙÚÝÄÕ");
	keyboard[4].push_back("ËÏÖÜŸÂÊÎÔÛÅÃÑ");
	keyboard[4].push_back("ČĎĚĽĹŇÔŘŔŠŤŮŽ");
	keyboard[4].push_back("ĄĆĘŁŃŚŻŹ     ");


	keyboard[5].push_back("æçабвгдеёжзий ");
	keyboard[5].push_back("клмнопрстуфхцч");
	keyboard[5].push_back("шщъыьэюяøðßÐÞþ");

	keyboard[6].push_back("ÆÇАБВГДЕЁЖЗИЙ ");
	keyboard[6].push_back("КЛМНОПРСТУФХЦЧ");
	keyboard[6].push_back("ШЩЪЫЬЭЮЯØðßÐÞþ");

	setKeyboard(0);

	buttonbox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/l.png",
			gmenu2x->tr["Backspace"],
			bind(&InputDialog::backspace, this))));

	buttonbox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/r.png",
			gmenu2x->tr["Space"],
			bind(&InputDialog::space, this))));

	buttonbox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/accept.png",
			gmenu2x->tr["Confirm"],
			bind(&InputDialog::confirm, this))));

	buttonbox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/cancel.png",
			gmenu2x->tr["Change keys"],
			bind(&InputDialog::changeKeys, this))));
}

void InputDialog::setKeyboard(int kb) {
	kb = constrain(kb, 0, keyboard.size() - 1);
	curKeyboard = kb;
	this->kb = &(keyboard[kb]);
	kbLength = this->kb->at(0).length();
	for (int x = 0, l = kbLength; x < l; x++) {
		if (utf8Code(this->kb->at(0)[x])) {
			kbLength--;
			x++;
		}
	}

	kbLeft = 160 - kbLength * KEY_WIDTH / 2;
	kbWidth = kbLength * KEY_WIDTH + 3;
	kbHeight = (this->kb->size() + 1) * KEY_HEIGHT + 3;

	kbRect.x = kbLeft - 3;
	kbRect.y = KB_TOP - 2;
	kbRect.w = kbWidth;
	kbRect.h = kbHeight;
}

bool InputDialog::exec() {
	SDL_Rect box = {
		0, 60, 0, static_cast<Uint16>(gmenu2x->font->getLineSpacing() + 4)
	};

	Uint32 caretTick = 0, curTick;
	bool caretOn = true;

	OffscreenSurface bg(*gmenu2x->bg);
	drawTitleIcon(bg, icon, false);
	writeTitle(bg, title);
	writeSubTitle(bg, text);
	buttonbox.paint(bg, 5, gmenu2x->resY - 1);
	bg.convertToDisplayFormat();

	close = false;
	ok = true;
	while (!close) {
		OutputSurface& s = *gmenu2x->s;

		bg.blit(s, 0, 0);

		box.w = gmenu2x->font->getTextWidth(input) + 18;
		box.x = 160 - box.w / 2;
		s.box(box.x, box.y, box.w, box.h,
		gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
		s.rectangle(box.x, box.y, box.w, box.h,
				gmenu2x->skinConfColors[COLOR_SELECTION_BG]);

		gmenu2x->font->write(s, input, box.x + 5, box.y + box.h - 2,
				Font::HAlignLeft, Font::VAlignBottom);

		curTick = SDL_GetTicks();
		if (curTick - caretTick >= 600) {
			caretOn = !caretOn;
			caretTick = curTick;
		}

		if (caretOn) {
			s.box(box.x + box.w - 12, box.y + 3, 8, box.h - 6,
					gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
		}

		if (ts.available()) ts.poll();
		drawVirtualKeyboard();
		s.flip();

		switch (inputMgr.waitForPressedButton()) {
			case InputManager::SETTINGS:
				ok = true;
				close = true;
				break;
			case InputManager::MENU:
				ok = false;
				close = true;
				break;
			case InputManager::UP:
				selRow--;
				break;
			case InputManager::DOWN:
				selRow++;
				if (selRow == (int)kb->size()) selCol = selCol < 8 ? 0 : 1;
				break;
			case InputManager::LEFT:
				selCol--;
				break;
			case InputManager::RIGHT:
				selCol++;
				break;
			case InputManager::ACCEPT:
				confirm();
				break;
			case InputManager::CANCEL:
				changeKeys();
				break;
			case InputManager::ALTLEFT:
				backspace();
				break;
			case InputManager::ALTRIGHT:
				space();
				break;
			default:
				break;
		}
	}

	return ok;
}

void InputDialog::backspace() {
	// Check for UTF8 characters.
	input = input.substr(0, input.length()
		- (utf8Code(input[input.length() - 2]) ? 2 : 1));
}

void InputDialog::space() {
	input += " ";
}

void InputDialog::confirm() {
	if (selRow == (int)kb->size()) {
		if (selCol == 0) {
			ok = false;
		}
		close = true;
	} else {
		int xc = 0;
		for (uint x = 0; x < kb->at(selRow).length(); x++) {
			bool utf8 = utf8Code(kb->at(selRow)[x]);
			if (xc == selCol) input += kb->at(selRow).substr(x, utf8 ? 2 : 1);
			if (utf8) x++;
			xc++;
		}
	}
}

void InputDialog::changeKeys() {
	if (curKeyboard == 6) {
		setKeyboard(0);
	} else {
		setKeyboard(curKeyboard + 1);
	}
}

void InputDialog::drawVirtualKeyboard() {
	Surface& s = *gmenu2x->s;

	//keyboard border
	s.rectangle(kbRect, gmenu2x->skinConfColors[COLOR_SELECTION_BG]);

	if (selCol<0) selCol = selRow==(int)kb->size() ? 1 : kbLength-1;
	if (selCol>=(int)kbLength) selCol = 0;
	if (selRow<0) selRow = kb->size()-1;
	if (selRow>(int)kb->size()) selRow = 0;

	//selection
	if (selRow<(int)kb->size())
		s.box(kbLeft + selCol * KEY_WIDTH - 1,
				KB_TOP + selRow * KEY_HEIGHT, KEY_WIDTH - 1, KEY_HEIGHT - 2,
				gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
	else {
		if (selCol > 1) selCol = 0;
		if (selCol < 0) selCol = 1;
		s.box(kbLeft + selCol * kbLength * KEY_WIDTH / 2 - 1,
				KB_TOP + kb->size() * KEY_HEIGHT, kbLength * KEY_WIDTH / 2 - 1,
				KEY_HEIGHT - 1, gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
	}

	//keys
	for (uint l=0; l<kb->size(); l++) {
		string line = kb->at(l);
		for (uint x=0, xc=0; x<line.length(); x++) {
			string charX;
			//utf8 characters
			if (utf8Code(line[x])) {
				charX = line.substr(x,2);
				x++;
			} else
				charX = line[x];

			SDL_Rect re = {
				static_cast<Sint16>(kbLeft + xc * KEY_WIDTH - 1),
				static_cast<Sint16>(KB_TOP + l * KEY_HEIGHT),
				KEY_WIDTH - 1,
				KEY_HEIGHT - 2
			};

			//if ts on rect, change selection
			if (ts.available() && ts.pressed() && ts.inRect(re)) {
				selCol = xc;
				selRow = l;
			}

			s.rectangle(re,
					gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
			gmenu2x->font->write(s, charX,
					kbLeft + xc * KEY_WIDTH + KEY_WIDTH / 2 - 1,
					KB_TOP + l * KEY_HEIGHT + KEY_HEIGHT / 2,
					Font::HAlignCenter, Font::VAlignMiddle);
			xc++;
		}
	}

	//Ok/Cancel
	SDL_Rect re = {
		static_cast<Sint16>(kbLeft - 1),
		static_cast<Sint16>(KB_TOP + kb->size() * KEY_HEIGHT),
		static_cast<Uint16>(kbLength * KEY_WIDTH / 2 - 1),
		KEY_HEIGHT - 1
	};
	s.rectangle(re, gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
	if (ts.available() && ts.pressed() && ts.inRect(re)) {
		selCol = 0;
		selRow = kb->size();
	}
	gmenu2x->font->write(s, gmenu2x->tr["Cancel"],
			(int)(160 - kbLength * KEY_WIDTH / 4),
			KB_TOP + kb->size() * KEY_HEIGHT + KEY_HEIGHT / 2,
			Font::HAlignCenter, Font::VAlignMiddle);

	re.x = kbLeft + kbLength * KEY_WIDTH / 2 - 1;
	s.rectangle(re, gmenu2x->skinConfColors[COLOR_SELECTION_BG]);
	if (ts.available() && ts.pressed() && ts.inRect(re)) {
		selCol = 1;
		selRow = kb->size();
	}
	gmenu2x->font->write(s, gmenu2x->tr["OK"],
			(int)(160 + kbLength * KEY_WIDTH / 4),
			KB_TOP + kb->size() * KEY_HEIGHT + KEY_HEIGHT / 2,
			Font::HAlignCenter, Font::VAlignMiddle);
}
