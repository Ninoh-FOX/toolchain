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

#include "menusetting.h"

#include "font.h"
#include "gmenu2x.h"
#include "surface.h"

using std::string;

MenuSetting::MenuSetting(GMenu2X *gmenu2x, const string &name,
		const string &description)
	: gmenu2x(gmenu2x)
	, name(name)
	, description(description)
{
}

MenuSetting::~MenuSetting()
{
}

void MenuSetting::draw(int /*valueX*/, int y, int /*h*/)
{
	Surface& s = *gmenu2x->s;
	gmenu2x->font->write(s, name, 5, y, Font::HAlignLeft, Font::VAlignTop);
}

void MenuSetting::handleTS(int /*valueX*/, int /*y*/, int /*h*/)
{
	buttonBox.handleTS();
}

void MenuSetting::drawSelected(int valueX, int y, int h)
{
	Surface& s = *gmenu2x->s;

	// The selection rectangle
	s.box(0, y, valueX - 5, h,
		gmenu2x->skinConfColors[COLOR_SELECTION_BG]);

	buttonBox.paint(s, 5, gmenu2x->resY - 1);
}
