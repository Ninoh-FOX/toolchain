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

#include "menusettingstring.h"

#include "gmenu2x.h"
#include "iconbutton.h"
#include "inputdialog.h"

using std::bind;
using std::string;
using std::unique_ptr;

MenuSettingString::MenuSettingString(
		GMenu2X *gmenu2x, Touchscreen &ts_,
		const string &name, const string &description, string *value,
		const string &diagTitle_, const string &diagIcon_)
	: MenuSettingStringBase(gmenu2x, name, description, value)
	, ts(ts_)
	, diagTitle(diagTitle_)
	, diagIcon(diagIcon_)
{
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/cancel.png",
			gmenu2x->tr["Clear"],
			bind(&MenuSettingString::clear, this))));

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/accept.png",
			gmenu2x->tr["Edit"],
			bind(&MenuSettingString::edit, this))));
}

void MenuSettingString::edit()
{
	InputDialog id(
			gmenu2x, gmenu2x->input, ts,
			description, value(), diagTitle, diagIcon);
	if (id.exec()) setValue(id.getInput());
}
