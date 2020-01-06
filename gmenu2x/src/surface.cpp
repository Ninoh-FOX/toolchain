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

#include "surface.h"

#include "debug.h"
#include "imageio.h"
#include "utilities.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <utility>

using namespace std;

// pixel operations
// getpixel
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

// pixel operations
// putpixel
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

// RGBAColor:

RGBAColor RGBAColor::fromString(const string &strColor) {
	return {
		uint8_t(constrain(strtol(strColor.substr(0, 2).c_str(), nullptr, 16),
		                  0, 255)),
		uint8_t(constrain(strtol(strColor.substr(2, 2).c_str(), nullptr, 16),
		                  0, 255)),
		uint8_t(constrain(strtol(strColor.substr(4, 2).c_str(), nullptr, 16),
		                  0, 255)),
		uint8_t(constrain(strtol(strColor.substr(6, 2).c_str(), nullptr, 16),
		                  0, 255)),
	};
}

ostream& operator<<(ostream& os, RGBAColor const& color) {
	auto oldfill = os.fill('0');
	auto oldflags = os.setf(ios::hex | ios::right,
	                        ios::basefield | ios::adjustfield);
	os << setw(2) << uint32_t(color.r)
	   << setw(2) << uint32_t(color.g)
	   << setw(2) << uint32_t(color.b)
	   << setw(2) << uint32_t(color.a);
	os.fill(oldfill);
	os.setf(oldflags);
	return os;
}


// Surface:

Surface::Surface(Surface const& other)
	: Surface(SDL_ConvertSurface(other.raw, other.raw->format, SDL_SWSURFACE))
{
	// Note: A bug in SDL_ConvertSurface() leaves the per-surface alpha
	//       undefined when converting from RGBA to RGBA. This can cause
	//       problems if the surface is later converted to a format without
	//       an alpha channel, such as the display format.
	raw->format->alpha = other.raw->format->alpha;
}

void Surface::blitScaled(Surface &destination, int x, int y, int w, int h, int a) const {
	if (a==0 || w==0 || h==0) return;

  if (a>0 && a!=raw->format->alpha)
		SDL_SetAlpha(raw, SDL_SRCALPHA|SDL_RLEACCEL, a);

  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  SDL_Surface *tempsrc=SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, bmask, gmask, rmask, amask);

  SDL_LockSurface(tempsrc);
  double srcx, srcy;
  double srcx_ratio=double(raw->w)/double(w);
  double srcy_ratio=double(raw->h)/double(h);
  for(int posy=0; posy<h; posy++) {
    for(int posx=0; posx<w; posx++) {
      srcx=posx*srcx_ratio;
      srcy=posy*srcy_ratio;
      putpixel(tempsrc,posx,posy,getpixel(raw,int(srcx),int(srcy)));
    }
  }
  SDL_UnlockSurface(tempsrc);


  SDL_Rect dest = { static_cast<Sint16>(x), static_cast<Sint16>(y), 0, 0};
  SDL_BlitSurface(tempsrc,NULL,destination.raw,&dest);
  SDL_FreeSurface(tempsrc);
}

void Surface::blit(SDL_Surface *destination, int x, int y, int w, int h, int a) const {
	if (destination == NULL || a==0) return;

	SDL_Rect src = { 0, 0, static_cast<Uint16>(w), static_cast<Uint16>(h) };
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	if (a>0 && a!=raw->format->alpha)
		SDL_SetAlpha(raw, SDL_SRCALPHA|SDL_RLEACCEL, a);
	SDL_BlitSurface(raw, (w==0 || h==0) ? NULL : &src, destination, &dest);
}
void Surface::blit(Surface& destination, int x, int y, int w, int h, int a) const {
	blit(destination.raw, x, y, w, h, a);
}

void Surface::blitCenter(SDL_Surface *destination, int x, int y, int w, int h, int a) const {
	int ow = raw->w / 2; if (w != 0) ow = min(ow, w / 2);
	int oh = raw->h / 2; if (h != 0) oh = min(oh, h / 2);
	blit(destination, x - ow, y - oh, w, h, a);
}
void Surface::blitCenter(Surface& destination, int x, int y, int w, int h, int a) const {
	blitCenter(destination.raw, x, y, w, h, a);
}

void Surface::blitRight(SDL_Surface *destination, int x, int y, int w, int h, int a) const {
	if (!w) w = raw->w;
	blit(destination, x - min(raw->w, w), y, w, h, a);
}
void Surface::blitRight(Surface& destination, int x, int y, int w, int h, int a) const {
	if (!w) w = raw->w;
	blitRight(destination.raw, x, y, w, h, a);
}

void Surface::box(SDL_Rect re, RGBAColor c) {
	if (c.a == 255) {
		SDL_FillRect(raw, &re, c.pixelValue(raw->format));
	} else if (c.a != 0) {
		fillRectAlpha(re, c);
	}
}

void Surface::rectangle(SDL_Rect re, RGBAColor c) {
	if (re.h >= 1) {
		// Top.
		box(SDL_Rect { re.x, re.y, re.w, 1 }, c);
	}
	if (re.h >= 2) {
		Sint16 ey = re.y + re.h - 1;
		// Bottom.
		box(SDL_Rect { re.x, ey, re.w, 1 }, c);

		Sint16 ex = re.x + re.w - 1;
		Sint16 sy = re.y + 1;
		Uint16 sh = re.h - 2;
		// Left.
		if (re.w >= 1) {
			box(SDL_Rect { re.x, sy, 1, sh }, c);
		}
		// Right.
		if (re.w >= 2) {
			box(SDL_Rect { ex, sy, 1, sh }, c);
		}
	}
}

void Surface::clearClipRect() {
	SDL_SetClipRect(raw,NULL);
}

void Surface::setClipRect(int x, int y, int w, int h) {
	SDL_Rect rect = {
		static_cast<Sint16>(x), static_cast<Sint16>(y),
		static_cast<Uint16>(w), static_cast<Uint16>(h)
	};
	setClipRect(rect);
}

void Surface::setClipRect(SDL_Rect rect) {
	SDL_SetClipRect(raw,&rect);
}

void Surface::applyClipRect(SDL_Rect& rect) {
	SDL_Rect clip;
	SDL_GetClipRect(raw, &clip);

	// Clip along X-axis.
	if (rect.x < clip.x) {
		rect.w = max(rect.x + rect.w - clip.x, 0);
		rect.x = clip.x;
	}
	if (rect.x + rect.w > clip.x + clip.w) {
		rect.w = max(clip.x + clip.w - rect.x, 0);
	}

	// Clip along Y-axis.
	if (rect.y < clip.y) {
		rect.h = max(rect.y + rect.h - clip.y, 0);
		rect.y = clip.y;
	}
	if (rect.y + rect.h > clip.y + clip.h) {
		rect.h = max(clip.y + clip.h - rect.y, 0);
	}
}

void Surface::blit(Surface& destination, SDL_Rect container, Font::HAlign halign, Font::VAlign valign) const {
	switch (halign) {
	case Font::HAlignLeft:
		break;
	case Font::HAlignCenter:
		container.x += container.w / 2 - raw->w / 2;
		break;
	case Font::HAlignRight:
		container.x += container.w-raw->w;
		break;
	}

	switch (valign) {
	case Font::VAlignTop:
		break;
	case Font::VAlignMiddle:
		container.y += container.h / 2 - raw->h / 2;
		break;
	case Font::VAlignBottom:
		container.y += container.h-raw->h;
		break;
	}

	blit(destination, container.x, container.y);
}

static inline uint32_t mult8x4(uint32_t c, uint8_t a) {
	return ((((c >> 8) & 0x00FF00FF) * a) & 0xFF00FF00)
	     | ((((c & 0x00FF00FF) * a) & 0xFF00FF00) >> 8);
}

void Surface::fillRectAlpha(SDL_Rect rect, RGBAColor c) {
	applyClipRect(rect);
	if (rect.w == 0 || rect.h == 0) {
		// Entire rectangle is outside clipping area.
		return;
	}

	if (SDL_MUSTLOCK(raw)) {
		if (SDL_LockSurface(raw) < 0) {
			return;
		}
	}

	SDL_PixelFormat *format = raw->format;
	uint32_t color = c.pixelValue(format);
	uint8_t alpha = c.a;

	uint8_t* edge = static_cast<uint8_t*>(raw->pixels)
	               + rect.y * raw->pitch
	               + rect.x * format->BytesPerPixel;

	// Blending: surf' = surf * (1 - alpha) + fill * alpha

	if (format->BytesPerPixel == 2) {
		uint32_t Rmask = format->Rmask;
		uint32_t Gmask = format->Gmask;
		uint32_t Bmask = format->Bmask;

		// Pre-multiply the fill color. We're hardcoding alpha to 1: 15/16bpp
		// modes are unlikely to have an alpha channel and even if they do,
		// the written alpha isn't used by gmenu2x.
		uint16_t f = (((color & Rmask) * alpha >> 8) & Rmask)
		           | (((color & Gmask) * alpha >> 8) & Gmask)
		           | (((color & Bmask) * alpha >> 8) & Bmask)
		           | format->Amask;
		alpha = 255 - alpha;

		for (auto y = 0; y < rect.h; y++) {
			for (auto x = 0; x < rect.w; x++) {
				uint16_t& pixel = reinterpret_cast<uint16_t*>(edge)[x];
				uint32_t R = ((pixel & Rmask) * alpha >> 8) & Rmask;
				uint32_t G = ((pixel & Gmask) * alpha >> 8) & Gmask;
				uint32_t B = ((pixel & Bmask) * alpha >> 8) & Bmask;
				pixel = uint16_t(R | G | B) + f;
			}
			edge += raw->pitch;
		}
	} else if (format->BytesPerPixel == 4) {
		// Assume the pixel format uses 8 bits per component; we don't care
		// which component is where since they all blend the same.
		uint32_t f = mult8x4(color, alpha); // pre-multiply the fill color
		alpha = 255 - alpha;

		for (auto y = 0; y < rect.h; y++) {
			for (auto x = 0; x < rect.w; x++) {
				uint32_t& pixel = reinterpret_cast<uint32_t*>(edge)[x];
				pixel = mult8x4(pixel, alpha) + f;
			}
			edge += raw->pitch;
		}
	} else {
		assert(false);
	}

	if (SDL_MUSTLOCK(raw)) {
		SDL_UnlockSurface(raw);
	}
}


// OffscreenSurface:

unique_ptr<OffscreenSurface> OffscreenSurface::emptySurface(
		int width, int height)
{
	SDL_Surface *raw = SDL_CreateRGBSurface(
			SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
	if (!raw) return unique_ptr<OffscreenSurface>();
	SDL_FillRect(raw, nullptr, SDL_MapRGB(raw->format, 0, 0, 0));
	return unique_ptr<OffscreenSurface>(new OffscreenSurface(raw));
}

unique_ptr<OffscreenSurface> OffscreenSurface::loadImage(
		string const& img, bool loadAlpha)
{
	SDL_Surface *raw = loadPNG(img, loadAlpha);
	if (!raw) {
		DEBUG("Couldn't load surface '%s'\n", img.c_str());
		return unique_ptr<OffscreenSurface>();
	}

	return unique_ptr<OffscreenSurface>(new OffscreenSurface(raw));
}

OffscreenSurface::OffscreenSurface(OffscreenSurface&& other)
	: Surface(other.raw)
{
	other.raw = nullptr;
}

OffscreenSurface::~OffscreenSurface()
{
	SDL_FreeSurface(raw);
}

OffscreenSurface& OffscreenSurface::operator=(OffscreenSurface other)
{
	swap(other);
	return *this;
}

void OffscreenSurface::swap(OffscreenSurface& other)
{
	std::swap(raw, other.raw);
}

void OffscreenSurface::convertToDisplayFormat() {
	SDL_Surface *newSurface = SDL_DisplayFormat(raw);
	if (newSurface) {
		SDL_FreeSurface(raw);
		raw = newSurface;
	}
}


// OutputSurface:

unique_ptr<OutputSurface> OutputSurface::open(
		int width, int height, int bitsPerPixel)
{
	SDL_ShowCursor(SDL_DISABLE);
	SDL_Surface *raw = SDL_SetVideoMode(
		width, height, bitsPerPixel, SDL_HWSURFACE | SDL_DOUBLEBUF);
	return unique_ptr<OutputSurface>(raw ? new OutputSurface(raw) : nullptr);
}

void OutputSurface::flip() {
	SDL_Flip(raw);
}
