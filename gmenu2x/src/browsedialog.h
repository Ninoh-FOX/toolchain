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

#ifndef BROWSEDIALOG_H
#define BROWSEDIALOG_H

#include "buttonbox.h"
#include "dialog.h"
#include "filelister.h"
#include "inputmanager.h"

#include <SDL.h>
#include <string>

class OffscreenSurface;
class Touchscreen;

class BrowseDialog : protected Dialog {
protected:
	BrowseDialog(
			GMenu2X *gmenu2x, Touchscreen &ts,
			const std::string &title, const std::string &subtitle);
	virtual ~BrowseDialog();

	void setPath(const std::string &path) {
		this->path = path;
		fl.browse(path);
	}

	FileLister fl;
	unsigned int selected;

private:
	enum Action {
		ACT_NONE,
		ACT_SELECT,
		ACT_CLOSE,
		ACT_UP,
		ACT_DOWN,
		ACT_SCROLLUP,
		ACT_SCROLLDOWN,
		ACT_GOUP,
		ACT_CONFIRM,
	};

	Touchscreen &ts;
	bool close, result;

	std::string title;
	std::string subtitle;
	std::string path;

	SDL_Rect clipRect;
	SDL_Rect touchRect;

	unsigned int numRows;
	unsigned int rowHeight;

	bool ts_pressed;

	OffscreenSurface *iconGoUp;
	OffscreenSurface *iconFolder;
	OffscreenSurface *iconFile;

	ButtonBox buttonBox;

	Action getAction(InputManager::Button button);
	void handleInput();

	void paint();

	void directoryUp();
	void directoryEnter();
	void confirm();
	void quit();

public:
	bool exec();

	const std::string &getPath() {
		return path;
	}
	std::string getFile() {
		return fl[selected];
	}
};

#endif // INPUTDIALOG_H
