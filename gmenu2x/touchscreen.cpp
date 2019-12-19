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

#include "touchscreen.h"

#include <fcntl.h>
#include <unistd.h>


Touchscreen::Touchscreen() {
	calibrated = false;
	wasPressed = false;
	_handled = false;
	x = 0;
	y = 0;
	startX = 0;
	startY = 0;
	event.x = 0;
	event.y = 0;
	event.pressure = 0;
	ts_fd = 0;
}

Touchscreen::~Touchscreen() {
	if (ts_fd > 0) {
		close(ts_fd);
	}
}

void Touchscreen::calibrate() {
	if (event.pressure == 0) {
		calibX = ((event.x - 200) * 320 / 3750) / 4;
		calibY = (((event.y - 200) * 240 / 3750)) / 4;
		calibrated = true;
	}
}

bool Touchscreen::poll() {
	wasPressed = pressed();
	SDL_PumpEvents();
	int mx, my;
	if (SDL_GetMouseState(&mx,&my) && SDL_BUTTON(1)) {
		x = mx;
		y = my;
		event.pressure = 1;
	} else {
		event.pressure = 0;
	}
	_handled = false;

	if (!wasPressed && pressed()) {
		startX = x;
		startY = y;
	}

	return pressed();
}

bool Touchscreen::handled() {
	return _handled;
}

void Touchscreen::setHandled() {
	wasPressed = false;
	_handled = true;
}

bool Touchscreen::pressed() {
	return !_handled && event.pressure > 0;
}

bool Touchscreen::released() {
	return !pressed() && wasPressed;
}

bool Touchscreen::inRect(int ix, int iy, int iw, int ih) {
	return !_handled &&
		(y >= iy) && (y <= iy + ih) && (x >= ix) && (x <= ix + iw);
}

bool Touchscreen::inRect(SDL_Rect r) {
	return inRect(r.x, r.y, r.w, r.h);
}

bool Touchscreen::startedInRect(int ix, int iy, int iw, int ih) {
	return !_handled &&
		(startY >= iy) && (startY <= iy + ih) &&
		(startX >= ix) && (startX <= ix + iw);
}

bool Touchscreen::startedInRect(SDL_Rect r) {
	return startedInRect(r.x, r.y, r.w, r.h);
}
