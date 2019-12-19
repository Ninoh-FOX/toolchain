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

#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "dialog.h"
#include "buttonbox.h"

#include <SDL.h>
#include <string>
#include <vector>

class InputManager;
class Touchscreen;

class InputDialog : protected Dialog {
public:
	InputDialog(GMenu2X *gmenu2x, InputManager &inputMgr, Touchscreen &ts,
			const std::string &text, const std::string &startvalue="",
			const std::string &title="", const std::string &icon="");

	bool exec();
	const std::string &getInput() { return input; }

private:
	void backspace();
	void space();
	void confirm();
	void changeKeys();

	void drawVirtualKeyboard();
	void setKeyboard(int);

	InputManager &inputMgr;
	Touchscreen &ts;
	int selRow, selCol;
	bool close, ok;
	std::string title, text, icon;
	short curKeyboard;
	std::vector<std::vector<std::string>> keyboard;
	std::vector<std::string> *kb;
	int kbLength, kbWidth, kbHeight, kbLeft;
	SDL_Rect kbRect;
	ButtonBox buttonbox;
	std::string input;
};

#endif // INPUTDIALOG_H
