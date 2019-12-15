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

#include "menusettingint.h"

#include "gmenu2x.h"
#include "iconbutton.h"
#include "surface.h"
#include "utilities.h"

#include <sstream>

using std::bind;
using std::string;
using std::stringstream;
using std::unique_ptr;

MenuSettingInt::MenuSettingInt(
		GMenu2X *gmenu2x, Touchscreen &ts,
		const string &name, const string &description,
		int *value, int min, int max, int increment)
	: MenuSetting(gmenu2x,name,description)
{
	_value = value;
	originalValue = *value;
	this->min = min;
	this->max = max;
	this->increment = increment;
	setValue(this->value());

	//Delegates
	IconButton::Action actionInc = bind(&MenuSettingInt::inc, this);
	IconButton::Action actionDec = bind(&MenuSettingInt::dec, this);

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/l.png",
			"", actionDec)));
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/left.png",
			gmenu2x->tr["Decrease"], actionDec)));

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/r.png",
			"", actionInc)));
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/right.png",
			gmenu2x->tr["Increase"], actionInc)));
}

void MenuSettingInt::draw(int valueX, int y, int h)
{
	Surface& s = *gmenu2x->s;
	MenuSetting::draw(valueX, y, h);
	gmenu2x->font->write(s, strvalue, valueX, y, Font::HAlignLeft, Font::VAlignTop);
}

bool MenuSettingInt::handleButtonPress(InputManager::Button button)
{
	switch (button) {
		case InputManager::LEFT:
			dec();
			break;
		case InputManager::RIGHT:
			inc();
			break;
		case InputManager::ALTLEFT:
			setValue(value() - 10 * increment);
			break;
		case InputManager::ALTRIGHT:
			setValue(value() + 10 * increment);
			break;
		default:
			return false;
	}
	return true;
}

void MenuSettingInt::inc()
{
	setValue(value() + increment);
}

void MenuSettingInt::dec()
{
	setValue(value() - increment);
}

void MenuSettingInt::setValue(int value)
{
	*_value = constrain(value,min,max);
	stringstream ss;
	ss << *_value;
	strvalue = "";
	ss >> strvalue;
}

int MenuSettingInt::value()
{
	return *_value;
}

bool MenuSettingInt::edited()
{
	return originalValue != value();
}
