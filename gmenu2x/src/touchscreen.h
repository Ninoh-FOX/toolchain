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

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <SDL.h>

#include <stdint.h>

typedef struct {
	uint16_t pressure;
	uint16_t x;
	uint16_t y;
	uint16_t pad;
	struct timeval stamp;
} TS_EVENT;

class Touchscreen {
public:
	Touchscreen();
	~Touchscreen();

	bool available() {
		return false;
	}

	bool poll();
	bool pressed();
	bool released();

	bool handled();
	void setHandled();

	bool inRect(int x, int y, int w, int h);
	bool inRect(SDL_Rect r);
	bool startedInRect(int x, int y, int w, int h);
	bool startedInRect(SDL_Rect r);

	int getX() { return x; }
	int getY() { return y; }

private:
	int ts_fd;
	bool calibrated, _handled;
	TS_EVENT event;
	int calibX, calibY;
	int x, y, startX, startY;
	bool wasPressed;

	void calibrate(/*TS_EVENT event*/);
};

#endif
