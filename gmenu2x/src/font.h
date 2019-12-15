// Original font class was replaced by an SDL_ttf based one by Paul Cercueil.
// License: GPL version 2 or later.

#ifndef FONT_H
#define FONT_H

#include <SDL_ttf.h>
#include <memory>
#include <string>

class Surface;

/**
 * Wrapper around a TrueType or other FreeType-supported font.
 * The wrapper is valid even if the font couldn't be loaded, but in that case
 * nothing will be drawn.
 */
class Font {
public:
	enum HAlign { HAlignLeft, HAlignRight,  HAlignCenter };
	enum VAlign { VAlignTop,  VAlignBottom, VAlignMiddle };

	/**
	 * Returns a newly created Font object for the default font.
	 */
	static std::unique_ptr<Font> defaultFont();

	Font(const std::string &path, unsigned int size);
	~Font();

	std::string wordWrap(const std::string &text, int width);

	int getTextWidth(const std::string& text);
	int getTextHeight(const std::string& text);

	int getLineSpacing()
	{
		return lineSpacing;
	}

	/**
	 * Draws a text on a surface in this font.
	 * @return The width of the text in pixels.
	 */
	int write(Surface& surface,
				const std::string &text, int x, int y,
				HAlign halign = HAlignLeft, VAlign valign = VAlignTop);

private:
	Font(TTF_Font *font);

	std::string wordWrapSingleLine(const std::string &text,
				size_t start, size_t end, int width);

	/**
	 * Draws a single line of text on a surface in this font.
	 * @return The width of the text in pixels.
	 */
	int writeLine(Surface& surface, std::string const& text,
				int x, int y, HAlign halign, VAlign valign);

	TTF_Font *font;
	int lineSpacing;
};

#endif /* FONT_H */
