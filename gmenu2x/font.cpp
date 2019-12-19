#include "font.h"

#include "debug.h"
#include "surface.h"
#include "utilities.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <vector>

/* TODO: Let the theme choose the font and font size */
#define TTF_FONT "/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf"
#define TTF_FONT_SIZE 12

using namespace std;

unique_ptr<Font> Font::defaultFont()
{
	return unique_ptr<Font>(new Font(TTF_FONT, TTF_FONT_SIZE));
}

Font::Font(const std::string &path, unsigned int size)
{
	font = nullptr;
	lineSpacing = 1;

	/* Note: TTF_Init and TTF_Quit perform reference counting, so call them
	 * both unconditionally for each font. */
	if (TTF_Init() < 0) {
		ERROR("Unable to init SDL_ttf library\n");
		return;
	}

	font = TTF_OpenFont(path.c_str(), size);
	if (!font) {
		ERROR("Unable to open font\n");
		TTF_Quit();
		return;
	}

	lineSpacing = TTF_FontLineSkip(font);
}

Font::~Font()
{
	if (font) {
		TTF_CloseFont(font);
		TTF_Quit();
	}
}

int Font::getTextWidth(const string &text)
{
	if (!font) {
		return 1;
	}

	int w;
	size_t pos = text.find('\n', 0);
	if (pos == string::npos) {
		TTF_SizeUTF8(font, text.c_str(), &w, nullptr);
		return w;
	} else {
		int maxWidth = 1;
		size_t prev = 0;
		do {
			TTF_SizeUTF8(font, text.substr(prev, pos - prev).c_str(), &w, nullptr);
			maxWidth = max(w, maxWidth);
			prev = pos + 1;
			pos = text.find('\n', prev);
		} while (pos != string::npos);
		TTF_SizeUTF8(font, text.substr(prev).c_str(), &w, nullptr);
		return max(w, maxWidth);
	}
}

string Font::wordWrap(const string &text, int width)
{
	const size_t len = text.length();
	string result;
	result.reserve(len);

	size_t start = 0;
	while (true) {
		size_t end = min(text.find('\n', start), len);
		result.append(wordWrapSingleLine(text, start, end, width));
		start = end + 1;
		if (start >= len) {
			break;
		}
		result.push_back('\n');
	}

	return result;
}

string Font::wordWrapSingleLine(const string &text, size_t start, size_t end, int width)
{
	string result;
	result.reserve(end - start);

	while (start != end) {
		/* Clean the end of the string, allowing lines that are indented at
		 * the start to stay as such. */
		string run = rtrim(text.substr(start, end - start));
		int runWidth = getTextWidth(run);

		if (runWidth > width) {
			size_t fits = 0, doesntFit = run.length();
			/* First guess: width / runWidth approximates the proportion of
			 * the run that should fit. */
			size_t guess = min(run.length(), (size_t) (doesntFit * ((float) width / runWidth)));
			/* Adjust that to fully include any partial UTF-8 character. */
			while (guess < run.length() && !isUTF8Starter(run[guess])) {
				guess++;
			}

			if (getTextWidth(run.substr(0, guess)) <= width) {
				fits = guess;
				doesntFit = fits;
				/* Prime doesntFit, which should be closer to 2 * fits than
				 * to run.length() / 2 if the run is long. */
				do {
					fits = doesntFit; // determined to fit by a previous iteration
					doesntFit = min(2 * fits, run.length());
					while (doesntFit < run.length() && !isUTF8Starter(run[doesntFit])) {
						doesntFit++;
					}
				} while (doesntFit < run.length() && getTextWidth(run.substr(0, doesntFit)) <= width);
			} else {
				doesntFit = guess;
			}

			/* End this loop when N full characters fit but N + 1 don't. */
			while (fits + 1 < doesntFit) {
				size_t guess = fits + (doesntFit - fits) / 2;
				if (!isUTF8Starter(run[guess])) {
					size_t oldGuess = guess;
					/* Adjust the guess to fully include a UTF-8 character. */
					for (size_t offset = 1; offset < (doesntFit - fits) / 2 - 1; offset++) {
						if (isUTF8Starter(run[guess - offset])) {
							guess -= offset;
							break;
						} else if (isUTF8Starter(run[guess + offset])) {
							guess += offset;
							break;
						}
					}
					/* If there's no such character, exit early. */
					if (guess == oldGuess) {
						break;
					}
				}
				if (getTextWidth(run.substr(0, guess)) <= width) {
					fits = guess;
				} else {
					doesntFit = guess;
				}
			}

			/* The run shall be split at the last space-separated word that
			 * fully fits, or otherwise at the last character that fits. */
			size_t lastSpace = run.find_last_of(" \t\r", fits);
			if (lastSpace != string::npos) {
				fits = lastSpace;
			}

			/* If 0 characters fit, we'll have to make 1 fit anyway, otherwise
			 * we're in for an infinite loop. This can happen if the font size
			 * is large. */
			if (fits == 0) {
				fits = 1;
				while (fits < run.length() && !isUTF8Starter(run[fits])) {
					fits++;
				}
			}

			result.append(rtrim(run.substr(0, fits))).append("\n");
			start = min(end, text.find_first_not_of(" \t\r", start + fits));
		} else {
			result.append(rtrim(run));
			start = end;
		}
	}

	return result;
}

int Font::getTextHeight(const string &text)
{
	int nLines = 1;
	size_t pos = 0;
	while ((pos = text.find('\n', pos)) != string::npos) {
		nLines++;
		pos++;
	}
	return nLines * getLineSpacing();
}

int Font::write(Surface& surface, const string &text,
			int x, int y, HAlign halign, VAlign valign)
{
	if (!font) {
		return 0;
	}

	size_t pos = text.find('\n', 0);
	if (pos == string::npos) {
		return writeLine(surface, text, x, y, halign, valign);
	} else {
		int maxWidth = 0;
		size_t prev = 0;
		do {
			maxWidth = max(maxWidth,
					writeLine(surface, text.substr(prev, pos - prev),
						x, y, halign, valign));
			y += lineSpacing;
			prev = pos + 1;
			pos = text.find('\n', prev);
		} while (pos != string::npos);
		return max(maxWidth,
				writeLine(surface, text.substr(prev), x, y, halign, valign));
	}
}

int Font::writeLine(Surface& surface, std::string const& text,
				int x, int y, HAlign halign, VAlign valign)
{
	if (text.empty()) {
		// SDL_ttf will return a nullptr when rendering the empty string.
		return 0;
	}

	switch (valign) {
	case VAlignTop:
		break;
	case VAlignMiddle:
		y -= lineSpacing / 2;
		break;
	case VAlignBottom:
		y -= lineSpacing;
		break;
	}

	SDL_Color color = { 0, 0, 0, 0 };
	SDL_Surface *s = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (!s) {
		ERROR("Font rendering failed for text \"%s\"\n", text.c_str());
		return 0;
	}
	const int width = s->w;

	switch (halign) {
	case HAlignLeft:
		break;
	case HAlignCenter:
		x -= width / 2;
		break;
	case HAlignRight:
		x -= width;
		break;
	}

	SDL_Rect rect = { (Sint16) x, (Sint16) (y - 1), 0, 0 };
	SDL_BlitSurface(s, NULL, surface.raw, &rect);

	/* Note: rect.x / rect.y are reset everytime because SDL_BlitSurface
	 * will modify them if negative */
	rect.x = x;
	rect.y = y + 1;
	SDL_BlitSurface(s, NULL, surface.raw, &rect);

	rect.x = x - 1;
	rect.y = y;
	SDL_BlitSurface(s, NULL, surface.raw, &rect);

	rect.x = x + 1;
	rect.y = y;
	SDL_BlitSurface(s, NULL, surface.raw, &rect);
	SDL_FreeSurface(s);

	rect.x = x;
	rect.y = y;
	color.r = 0xff;
	color.g = 0xff;
	color.b = 0xff;

	s = TTF_RenderUTF8_Blended(font, text.c_str(), color);
	if (!s) {
		ERROR("Font rendering failed for text \"%s\"\n", text.c_str());
		return width;
	}
	SDL_BlitSurface(s, NULL, surface.raw, &rect);
	SDL_FreeSurface(s);

	return width;
}
