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
#ifndef MENUSETTING_H
#define MENUSETTING_H

#include "buttonbox.h"
#include "inputmanager.h"

#include <string>

/**
Base class for different kind of option

	@author Massimiliano Torromeo <massimiliano.torromeo@gmail.com>
*/
class MenuSetting {
protected:
	GMenu2X *gmenu2x;

	ButtonBox buttonBox;

	std::string name, description;

public:
	MenuSetting(
			GMenu2X *gmenu2x, const std::string &name,
			const std::string &description);
	virtual ~MenuSetting();

	virtual void draw(int valueX, int y, int h);
	virtual void handleTS(int valueX, int y, int h);

	virtual bool handleButtonPress(InputManager::Button button) = 0;
	virtual void drawSelected(int valueX, int y, int h);
	virtual bool edited() = 0;

	const std::string &getName() { return name; }
	const std::string &getDescription() { return description; }
};

#endif
