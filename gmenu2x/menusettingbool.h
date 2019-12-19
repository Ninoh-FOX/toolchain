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
#ifndef MENUSETTINGBOOL_H
#define MENUSETTINGBOOL_H

#include "menusetting.h"
#include "inputmanager.h"

class GMenu2X;
class Touchscreen;

class MenuSettingBool : public MenuSetting {
private:
	void initButton();
	void toggle();

	Touchscreen &ts;
	bool originalValue;
	bool *_value;
	int *_ivalue;
	std::string strvalue;

public:
	MenuSettingBool(
			GMenu2X *gmenu2x, Touchscreen &ts,
			const std::string &name, const std::string &description,
			bool *value);
	MenuSettingBool(
			GMenu2X *gmenu2x, Touchscreen &ts,
			const std::string &name, const std::string &description,
			int *value);
	virtual ~MenuSettingBool() {}

	virtual void draw(int valueX, int y, int h);
	virtual bool handleButtonPress(InputManager::Button button);
	virtual bool edited();

	void setValue(int value);
	void setValue(bool value);
	bool value();
};

#endif
