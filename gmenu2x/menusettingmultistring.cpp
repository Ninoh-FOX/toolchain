/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo   *
 *   massimiliano.torromeo@gmail.com   *
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

#include "menusettingmultistring.h"
#include "gmenu2x.h"
#include "iconbutton.h"

#include <algorithm>

using std::bind;
using std::find;
using std::string;
using std::vector;
using std::unique_ptr;

MenuSettingMultiString::MenuSettingMultiString(
		GMenu2X *gmenu2x, Touchscreen &ts,
		const string &name, const string &description,
		string *value, const vector<string> *choices_)
	: MenuSettingStringBase(gmenu2x, name, description, value)
	, choices(choices_)
{
	setSel(find(choices->begin(), choices->end(), *value) - choices->begin());

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/left.png", "",
			bind(&MenuSettingMultiString::decSel, this))));
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/right.png",
			gmenu2x->tr["Change value"],
			bind(&MenuSettingMultiString::incSel, this))));
}

bool MenuSettingMultiString::handleButtonPress(InputManager::Button button)
{
	switch (button) {
		case InputManager::LEFT:
			decSel();
			break;
		case InputManager::RIGHT:
			incSel();
			break;
		default:
			return false;
	}
	return true;
}

void MenuSettingMultiString::incSel()
{
	setSel(selected + 1);
}

void MenuSettingMultiString::decSel()
{
	setSel(selected - 1);
}

void MenuSettingMultiString::setSel(int sel)
{
	if (sel < 0) {
		sel = choices->size()-1;
	} else if (sel >= (int)choices->size()) {
		sel = 0;
	}
	selected = sel;
	setValue((*choices)[sel]);
}
