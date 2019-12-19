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

#ifndef TEXTMANUALDIALOG_H
#define TEXTMANUALDIALOG_H

#include "textdialog.h"

#include <string>
#include <vector>

struct ManualPage {
	std::string title;
	std::vector<std::string> text;
};

class TextManualDialog : public TextDialog {
private:
	std::vector<ManualPage> pages;

public:
	TextManualDialog(GMenu2X *gmenu2x, const std::string &title,
			const std::string &icon, const std::string &text);
	void exec();
};

#endif // TEXTMANUALDIALOG_H
