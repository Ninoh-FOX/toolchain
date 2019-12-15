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

#include "textdialog.h"

#include "gmenu2x.h"
#include "utilities.h"

#include <algorithm>

using namespace std;

TextDialog::TextDialog(GMenu2X *gmenu2x, const string &title, const string &description, const string &icon, const string &text)
	: Dialog(gmenu2x)
{
	split(this->text, gmenu2x->font->wordWrap(text, (int) gmenu2x->resX - 15), "\n");
	this->title = title;
	this->description = description;
	this->icon = icon;
}

void TextDialog::drawText(const vector<string> &text, unsigned int y,
		unsigned int firstRow, unsigned int rowsPerPage)
{
	Surface& s = *gmenu2x->s;
	const int fontHeight = gmenu2x->font->getLineSpacing();

	for (unsigned i = firstRow; i < firstRow + rowsPerPage && i < text.size(); i++) {
		const string &line = text.at(i);
		int rowY = y + (i - firstRow) * fontHeight;
		if (line == "----") { // horizontal ruler
			rowY += fontHeight / 2;
			s.box(5, rowY, gmenu2x->resX - 16, 1, 255, 255, 255, 130);
			s.box(5, rowY+1, gmenu2x->resX - 16, 1, 0, 0, 0, 130);
		} else {
			gmenu2x->font->write(s, line, 5, rowY);
		}
	}

	gmenu2x->drawScrollBar(rowsPerPage, text.size(), firstRow);
}

void TextDialog::exec() {
	bool close = false;

	OffscreenSurface bg(*gmenu2x->bg);

	//link icon
	if (!fileExists(icon))
		drawTitleIcon(bg, "icons/ebook.png", true);
	else
		drawTitleIcon(bg, icon, false);
	writeTitle(bg, title);
	writeSubTitle(bg, description);

	int x = 5;
	x = gmenu2x->drawButton(bg, "up", "", x);
	x = gmenu2x->drawButton(bg, "down", gmenu2x->tr["Scroll"], x);
	x = gmenu2x->drawButton(bg, "cancel", "", x);
	x = gmenu2x->drawButton(bg, "start", gmenu2x->tr["Exit"], x);

	bg.convertToDisplayFormat();

	const int fontHeight = gmenu2x->font->getLineSpacing();
	unsigned int contentY, contentHeight;
	tie(contentY, contentHeight) = gmenu2x->getContentArea();
	const unsigned rowsPerPage = max(contentHeight / fontHeight, 1u);
	const unsigned maxFirstRow =
			text.size() < rowsPerPage ? 0 : text.size() - rowsPerPage;
	contentY += (contentHeight % fontHeight) / 2;

	unsigned firstRow = 0;
	while (!close) {
		OutputSurface& s = *gmenu2x->s;

		bg.blit(s, 0, 0);
		drawText(text, contentY, firstRow, rowsPerPage);
		s.flip();

		switch(gmenu2x->input.waitForPressedButton()) {
			case InputManager::UP:
				if (firstRow > 0) firstRow--;
				break;
			case InputManager::DOWN:
				if (firstRow < maxFirstRow) firstRow++;
				break;
			case InputManager::ALTLEFT:
				if (firstRow >= rowsPerPage - 1) firstRow -= rowsPerPage - 1;
				else firstRow = 0;
				break;
			case InputManager::ALTRIGHT:
				firstRow = min(firstRow + rowsPerPage - 1, maxFirstRow);
				break;
			case InputManager::SETTINGS:
			case InputManager::CANCEL:
				close = true;
				break;
			default:
				break;
		}
	}
}
