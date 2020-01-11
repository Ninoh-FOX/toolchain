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

#include "selector.h"

#include "debug.h"
#include "filelister.h"
#include "gmenu2x.h"
#include "linkapp.h"
#include "menu.h"
#include "surface.h"
#include "utilities.h"

#include <SDL.h>
#include <algorithm>

//for browsing the filesystem
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

using namespace std;

Selector::Selector(GMenu2X *gmenu2x, LinkApp& link, const string &selectorDir)
	: Dialog(gmenu2x)
	, link(link)
{
	dir = selectorDir.empty() ? link.getSelectorDir() : selectorDir;
	if (dir[dir.length()-1]!='/') dir += "/";
}

int Selector::exec(int startSelection) {
	const bool showDirectories = link.getSelectorBrowser();

	FileLister fl;
	fl.setShowDirectories(showDirectories);
	fl.setFilter(link.getSelectorFilter());
	while (!prepare(fl) && showDirectories && dir != "/") {
		// The given directory could not be opened; try parent.
		dir = parentDir(dir);
	}

	OffscreenSurface bg(*gmenu2x->bg);
	drawTitleIcon(bg, link.getIconPath(), true);
	writeTitle(bg, link.getTitle());
	writeSubTitle(bg, link.getDescription());

	int x = 5;
	if (fl.size() != 0) {
		x = gmenu2x->drawButton(bg, "accept", gmenu2x->tr["Select"], x);
	}
	if (showDirectories) {
		x = gmenu2x->drawButton(bg, "left", "", x);
		x = gmenu2x->drawButton(bg, "cancel", gmenu2x->tr["Up one folder"], x);
	} else {
		x = gmenu2x->drawButton(bg, "cancel", "", x);
	}
	x = gmenu2x->drawButton(bg, "start", gmenu2x->tr["Exit"], x);

	unsigned int top, height;
	tie(top, height) = gmenu2x->getContentArea();

	auto folderIcon = gmenu2x->sc.skinRes("imgs/folder.png");

	// Figure out how many items we can fit in the content area.
	int lineHeight = gmenu2x->font->getLineSpacing();
	if (showDirectories && folderIcon) {
		lineHeight = max(lineHeight, folderIcon->height() + 2);
	}
	unsigned int nb_elements = max(height / lineHeight, 1u);
	// Redistribute any leftover space.
	lineHeight = height / nb_elements;
	top += (height - lineHeight * nb_elements) / 2;

	bg.convertToDisplayFormat();

	unsigned int firstElement = 0;
	unsigned int selected = constrain(startSelection, 0, fl.size() - 1);

	bool close = false, result = true;
	while (!close) {
		OutputSurface& s = *gmenu2x->s;

		bg.blit(s, 0, 0);

		if (fl.size() == 0) {
			gmenu2x->font->write(s, "(" + gmenu2x->tr["no items"] + ")",
					4, top + lineHeight / 2,
					Font::HAlignLeft, Font::VAlignMiddle);
		} else {
			if (selected >= firstElement + nb_elements)
				firstElement = selected - nb_elements + 1;
			if (selected < firstElement)
				firstElement = selected;

			//Screenshot
			if (fl.isFile(selected)) {
				string path = screendir + trimExtension(fl[selected]) + ".png";
				auto screenshot = OffscreenSurface::loadImage(path, false);
				if (screenshot) {
					if(gmenu2x->confInt["previewType"]==1) {
            screenshot->blitRight(s, 320, 0, 320, 240, 128u);   // background preview
          } else {
            #define PREVIEW_RATIO   0.45      // preview scale, 1=240 height
            int h=240*PREVIEW_RATIO;
            int w=h * screenshot->width() / screenshot->height();
            screenshot->blitScaled(s, 315-w, 0, w, h);          // thumbnail preview
					}
				}
			}

			//Selection
			int iY = top + (selected - firstElement) * lineHeight;
			if (selected<fl.size())
				s.box(1, iY, 309, lineHeight, gmenu2x->skinConfColors[COLOR_SELECTION_BG]);

			//Files & Dirs
			s.setClipRect(0, top, 311, height);
			for (unsigned int i = firstElement;
					i < fl.size() && i < firstElement + nb_elements; i++) {
				iY = top + (i - firstElement) * lineHeight;
				x = 4;
				if (fl.isDirectory(i)) {
					if (folderIcon) {
						folderIcon->blit(s,
								x, iY + (lineHeight - folderIcon->height()) / 2);
						x += folderIcon->width() + 2;
					}
					gmenu2x->font->write(s, fl[i],
							x, iY + lineHeight / 2,
							Font::HAlignLeft, Font::VAlignMiddle);
				} else {
					gmenu2x->font->write(s, trimExtension(fl[i]),
							x, iY + lineHeight / 2,
							Font::HAlignLeft, Font::VAlignMiddle);
				}
			}
			s.clearClipRect();
		}

		gmenu2x->drawScrollBar(nb_elements, fl.size(), firstElement);
		s.flip();

		switch (gmenu2x->input.waitForPressedButton()) {
			case InputManager::SETTINGS:
				close = true;
				result = false;
				break;

			case InputManager::UP:
				if (selected == 0) selected = fl.size() -1;
				else selected -= 1;
				break;

			case InputManager::ALTLEFT:
				if ((int)(selected - nb_elements + 1) < 0)
					selected = 0;
				else
					selected -= nb_elements - 1;
				break;

			case InputManager::DOWN:
				if (selected+1>=fl.size()) selected = 0;
				else selected += 1;
				break;

			case InputManager::ALTRIGHT:
				if (selected + nb_elements - 1 >= fl.size())
					selected = fl.size() - 1;
				else
					selected += nb_elements - 1;
				break;

			case InputManager::CANCEL:
				if (!showDirectories) {
					close = true;
					result = false;
					break;
				}
				// ...fall through...
			case InputManager::LEFT:
				if (showDirectories) {
					selected = goToParentDir(fl);
					firstElement = 0;
				}
				break;

			case InputManager::ACCEPT:
				if (fl.size() != 0) {
					if (fl.isFile(selected)) {
						file = fl[selected];
						close = true;
					} else {
						string subdir = fl[selected];
						if (subdir == "..") {
							selected = goToParentDir(fl);
						} else {
							dir += subdir;
							char *buf = realpath(dir.c_str(), NULL);
							dir = (string) buf + '/';
							free(buf);

							prepare(fl);
							selected = 0;
						}
						firstElement = 0;
					}
				}
				break;

			default:
				break;
		}
	}

	return result ? (int)selected : -1;
}

bool Selector::prepare(FileLister& fl) {
	bool opened = fl.browse(dir);

	screendir = dir;
	if (!screendir.empty() && screendir[screendir.length() - 1] != '/') {
		screendir += "/";
	}
	screendir += ".previews/";

	return opened;
}

int Selector::goToParentDir(FileLister& fl) {
	string oldDir = dir;
	dir = parentDir(dir);
	prepare(fl);
	string oldName = oldDir.substr(dir.size(), oldDir.size() - dir.size() - 1);
	auto& subdirs = fl.getDirectories();
	auto it = find(subdirs.begin(), subdirs.end(), oldName);
	return it == subdirs.end() ? 0 : it - subdirs.begin();
}
