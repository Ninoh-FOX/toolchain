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

#include "textmanualdialog.h"

#include "gmenu2x.h"
#include "surface.h"
#include "utilities.h"

#include <algorithm>
#include <sstream>

using namespace std;

TextManualDialog::TextManualDialog(GMenu2X *gmenu2x, const string &title, const string &icon, const string &text)
	: TextDialog(gmenu2x,title,"",icon,text) {

	//split the text in multiple pages
	for (uint i=0; i<this->text.size(); i++) {
		string line = trim(this->text.at(i));
		if (line[0]=='[' && line[line.length()-1]==']') {
			ManualPage mp;
			mp.title = line.substr(1,line.length()-2);
			pages.push_back(mp);
		} else {
			if (pages.size()==0) {
				ManualPage mp;
				mp.title = gmenu2x->tr["Untitled"];
				pages.push_back(mp);
			}
			pages[pages.size()-1].text.push_back(this->text.at(i));
		}
	}
	if (pages.size()==0) {
		ManualPage mp;
		mp.title = gmenu2x->tr["Untitled"];
		pages.push_back(mp);
	}

	//delete first and last blank lines from each page
	for (uint page=0; page<pages.size(); page++) {
		if (pages[page].text.size() > 0) {
			//first lines
			while (trim(pages[page].text[0]).empty())
				pages[page].text.erase(pages[page].text.begin());
			//last lines
			while (trim(pages[page].text[pages[page].text.size()-1]).empty())
				pages[page].text.erase(pages[page].text.end());
		}
	}
}

void TextManualDialog::exec() {
	OffscreenSurface bg(*gmenu2x->bg);

	//link icon
	if (!fileExists(icon))
		drawTitleIcon(bg, "icons/ebook.png", true);
	else
		drawTitleIcon(bg, icon, false);
	writeTitle(bg, title+(description.empty() ? "" : ": "+description));

	int x = 5;
	x = gmenu2x->drawButton(bg, "up", "", x);
	x = gmenu2x->drawButton(bg, "down", gmenu2x->tr["Scroll"], x);
	x = gmenu2x->drawButton(bg, "left", "", x);
	x = gmenu2x->drawButton(bg, "right", gmenu2x->tr["Change page"], x);
	x = gmenu2x->drawButton(bg, "cancel", "", x);
	x = gmenu2x->drawButton(bg, "start", gmenu2x->tr["Exit"], x);

	bg.convertToDisplayFormat();

	stringstream ss;
	ss << pages.size();
	string spagecount;
	ss >> spagecount;
	string pageStatus;

	const int fontHeight = gmenu2x->font->getLineSpacing();
	unsigned int contentY, contentHeight;
	tie(contentY, contentHeight) = gmenu2x->getContentArea();
	const unsigned rowsPerPage = max(contentHeight / fontHeight, 1u);
	contentY += (contentHeight % fontHeight) / 2;

	unsigned page = 0, firstRow = 0;
	bool close = false;

	while (!close) {
		OutputSurface& s = *gmenu2x->s;

		bg.blit(s,0,0);
		writeSubTitle(s, pages[page].title);
		drawText(pages[page].text, contentY, firstRow, rowsPerPage);

		ss.clear();
		ss << page+1;
		ss >> pageStatus;
		pageStatus = gmenu2x->tr["Page"]+": "+pageStatus+"/"+spagecount;
		gmenu2x->font->write(s, pageStatus, 310, 230, Font::HAlignRight, Font::VAlignMiddle);

		s.flip();

		const unsigned maxFirstRow = pages[page].text.size() < rowsPerPage
				? 0 : pages[page].text.size() - rowsPerPage;

		switch(gmenu2x->input.waitForPressedButton()) {
			case InputManager::UP:
				if (firstRow > 0) firstRow--;
				break;
			case InputManager::DOWN:
				if (firstRow < maxFirstRow) firstRow++;
				break;
			case InputManager::LEFT:
				if (page > 0) {
					page--;
					firstRow = 0;
				}
				break;
			case InputManager::RIGHT:
				if (page < pages.size() -1) {
					page++;
					firstRow = 0;
				}
				break;
			case InputManager::ALTLEFT:
				if (firstRow >= rowsPerPage - 1) firstRow -= rowsPerPage - 1;
				else firstRow = 0;
				break;
			case InputManager::ALTRIGHT:
				firstRow = min(firstRow + rowsPerPage - 1, maxFirstRow);
				break;
			case InputManager::CANCEL:
			case InputManager::SETTINGS:
				close = true;
				break;
			default:
				break;
		}
	}
}
