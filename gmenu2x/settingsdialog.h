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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "dialog.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class InputManager;
class MenuSetting;
class Touchscreen;

class SettingsDialog : protected Dialog {
private:
	InputManager &inputMgr;
	Touchscreen &ts;
	std::vector<std::unique_ptr<MenuSetting>> settings;
	std::string text, icon;

public:
	SettingsDialog(GMenu2X *gmenu2x, InputManager &inputMgr, Touchscreen &ts,
			const std::string &text,
			const std::string &icon = "skin:sections/settings.png");

	void addSetting(std::unique_ptr<MenuSetting> setting) {
		settings.push_back(std::move(setting));
	}

	/**
	 * Shows the dialog in a modal way.
	 * @return true iff any settings were changed.
	 */
	bool exec();
};

#endif // SETTINGSDIALOG_H
