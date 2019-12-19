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

#ifndef SELECTOR_H
#define SELECTOR_H

#include "dialog.h"

#include <string>
#include <unordered_map>
#include <vector>

class LinkApp;
class FileLister;

class Selector : protected Dialog {
private:
	LinkApp& link;
	std::string file, dir, screendir;

	bool prepare(FileLister& fl);

	/**
	 * Changes 'dir' to its parent directory.
	 * Returns the index of the old dir in the parent, or 0 if unknown.
	 */
	int goToParentDir(FileLister& fl);

public:
	Selector(GMenu2X *gmenu2x, LinkApp& link,
			const std::string &selectorDir = "");

	int exec(int startSelection = 0);

	const std::string &getFile() { return file; }
	const std::string &getDir() { return dir; }
};

#endif // SELECTOR_H
