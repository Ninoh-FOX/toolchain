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

#include "menusettingbool.h"

#include "gmenu2x.h"
#include "iconbutton.h"
#include "surface.h"
#include "utilities.h"

#include <sstream>

using std::bind;
using std::string;
using std::unique_ptr;

MenuSettingBool::MenuSettingBool(
		GMenu2X *gmenu2x, Touchscreen &ts,
		const string &name, const string &description, int *value)
	: MenuSetting(gmenu2x, name, description)
	, ts(ts)
{
	_ivalue = value;
	_value = NULL;
	originalValue = *value != 0;
	setValue(this->value());
	initButton();
}

MenuSettingBool::MenuSettingBool(
		GMenu2X *gmenu2x, Touchscreen &ts,
		const string &name, const string &description, bool *value)
	: MenuSetting(gmenu2x, name, description)
	, ts(ts)
{
	_value = value;
	_ivalue = NULL;
	originalValue = *value;
	setValue(this->value());
	initButton();
}

void MenuSettingBool::initButton()
{
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/accept.png",
			gmenu2x->tr["Switch"],
			bind(&MenuSettingBool::toggle, this))));
}

void MenuSettingBool::draw(int valueX, int y, int h)
{
	Surface& s = *gmenu2x->s;
	MenuSetting::draw(valueX, y, h);
	gmenu2x->font->write(s, strvalue, valueX, y, Font::HAlignLeft, Font::VAlignTop);
}

bool MenuSettingBool::handleButtonPress(InputManager::Button button)
{
	switch (button) {
		case InputManager::ACCEPT:
		case InputManager::LEFT:
		case InputManager::RIGHT:
			toggle();
			break;
		default:
			return false;
	}
	return true;
}

void MenuSettingBool::toggle()
{
	setValue(!value());
}

void MenuSettingBool::setValue(int value)
{
	setValue(value != 0);
}

void MenuSettingBool::setValue(bool value)
{
	if (_value == NULL)
		*_ivalue = value;
	else
		*_value = value;
	strvalue = value ? "ON" : "OFF";
}

bool MenuSettingBool::value()
{
	if (_value == NULL)
		return *_ivalue != 0;
	else
		return *_value;
}

bool MenuSettingBool::edited()
{
	return originalValue != value();
}
