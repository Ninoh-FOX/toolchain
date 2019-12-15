#ifndef IMAGEIO_H
#define IMAGEIO_H

#include <string>

struct SDL_Surface;

/** Loads an image from a PNG file into a newly allocated 32bpp RGBA surface.
  */
SDL_Surface *loadPNG(const std::string &path, bool loadAlpha = true);

#endif
