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

#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include "dialog.h"

#include <string>
#include <vector>

class TextDialog : protected Dialog {
protected:
	std::vector<std::string> text;
	std::string title, description, icon;

	void drawText(const std::vector<std::string> &text, unsigned int y,
			unsigned int firstRow, unsigned int rowsPerPage);

public:
	TextDialog(GMenu2X *gmenu2x, const std::string &title,
			const std::string &description, const std::string &icon,
			const std::string &text);
	void exec();
};

#endif // TEXTDIALOG_H
