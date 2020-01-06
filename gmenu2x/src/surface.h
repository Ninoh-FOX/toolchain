/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo                           *
 *                         massimiliano.torromeo@gmail.com                 *
 *   Copyright (C) 2010-2014 by various authors; see Git log               *
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

#ifndef SURFACE_H
#define SURFACE_H

#include "font.h"

#include <SDL.h>

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

struct RGBAColor {
	uint8_t r, g, b, a;
	static RGBAColor fromString(std::string const& strColor);
	RGBAColor() : r(0), g(0), b(0), a(0) {}
	RGBAColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r(r), g(g), b(b), a(a) {}
	Uint32 pixelValue(SDL_PixelFormat *fmt) const {
		return SDL_MapRGBA(fmt, r, g, b, a);
	}
};
std::ostream& operator<<(std::ostream& os, RGBAColor const& color);

/**
 * Abstract base class for surfaces; wraps SDL_Surface.
 */
class Surface {
public:
	Surface& operator=(Surface const& other) = delete;

	int width() const { return raw->w; }
	int height() const { return raw->h; }

	void clearClipRect();
	void setClipRect(int x, int y, int w, int h);
	void setClipRect(SDL_Rect rect);

	void blitScaled(Surface &destination, int x, int y, int w, int h, int a=-1) const;
	void blit(Surface& destination, int x, int y, int w=0, int h=0, int a=-1) const;
	void blit(Surface& destination, SDL_Rect container, Font::HAlign halign = Font::HAlignLeft, Font::VAlign valign = Font::VAlignTop) const;
	void blitCenter(Surface& destination, int x, int y, int w=0, int h=0, int a=-1) const;
	void blitRight(Surface& destination, int x, int y, int w=0, int h=0, int a=-1) const;

	void box(SDL_Rect re, RGBAColor c);
	void box(Sint16 x, Sint16 y, Uint16 w, Uint16 h, RGBAColor c) {
		box((SDL_Rect){ x, y, w, h }, c);
	}
	void box(Sint16 x, Sint16 y, Uint16 w, Uint16 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		box((SDL_Rect){ x, y, w, h }, RGBAColor(r, g, b, a));
	}
	void rectangle(SDL_Rect re, RGBAColor c);
	void rectangle(Sint16 x, Sint16 y, Uint16 w, Uint16 h, RGBAColor c) {
		rectangle((SDL_Rect){ x, y, w, h }, c);
	}
	void rectangle(Sint16 x, Sint16 y, Uint16 w, Uint16 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		rectangle((SDL_Rect){ x, y, w, h }, RGBAColor(r, g, b, a));
	}

protected:
	Surface(SDL_Surface *raw) : raw(raw) {}
	Surface(Surface const& other);

	SDL_Surface *raw;

	// For direct access to "raw".
	friend class Font;

private:
	void blit(SDL_Surface *destination, int x, int y, int w=0, int h=0, int a=-1) const;
	void blitCenter(SDL_Surface *destination, int x, int y, int w=0, int h=0, int a=-1) const;
	void blitRight(SDL_Surface *destination, int x, int y, int w=0, int h=0, int a=-1) const;

	/** Draws the given rectangle on this surface in the given color, blended
	  * according to the alpha value of the color argument.
	  */
	void fillRectAlpha(SDL_Rect rect, RGBAColor c);

	/** Clips the given rectangle against this surface's active clipping
	  * rectangle.
	  */
	void applyClipRect(SDL_Rect& rect);
};

/**
 * A surface that is off-screen: not visible.
 */
class OffscreenSurface: public Surface {
public:
	static std::unique_ptr<OffscreenSurface> emptySurface(
			int width, int height);
	static std::unique_ptr<OffscreenSurface> loadImage(
			std::string const& img, bool loadAlpha = true);

	OffscreenSurface(Surface const& other) : Surface(other) {}
	OffscreenSurface(OffscreenSurface const& other) : Surface(other) {}
	OffscreenSurface(OffscreenSurface&& other);
	~OffscreenSurface();
	OffscreenSurface& operator=(OffscreenSurface other);
	void swap(OffscreenSurface& other);

	/**
	 * Converts the underlying surface to the same pixel format as the frame
	 * buffer, for faster blitting. This removes the alpha channel if the
	 * image has one.
	 */
	void convertToDisplayFormat();

private:
	OffscreenSurface(SDL_Surface *raw) : Surface(raw) {}
};

/**
 * A surface that is used for writing to a video output device.
 */
class OutputSurface: public Surface {
public:
	static std::unique_ptr<OutputSurface> open(
			int width, int height, int bitsPerPixel);

	/**
	 * Offers the current buffer to the video system to be presented and
	 * acquires a new buffer to draw into.
	 */
	void flip();

private:
	OutputSurface(SDL_Surface *raw) : Surface(raw) {}
};

#endif
