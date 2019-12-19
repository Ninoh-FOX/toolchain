#include <string>

#include "dialog.h"
#include "gmenu2x.h"
#include "font.h"

Dialog::Dialog(GMenu2X *gmenu2x) : gmenu2x(gmenu2x)
{
}

void Dialog::drawTitleIcon(Surface& s, const std::string &icon, bool skinRes)
{
	Surface *i = NULL;
	if (!icon.empty()) {
		if (skinRes)
			i = gmenu2x->sc.skinRes(icon);
		else
			i = gmenu2x->sc[icon];
	}

	if (i==NULL)
		i = gmenu2x->sc.skinRes("icons/generic.png");

	i->blit(s, 4, (gmenu2x->skinConfInt["topBarHeight"] - 32) / 2);
}

void Dialog::writeTitle(Surface& s, const std::string &title)
{
	gmenu2x->font->write(s, title, 40, 0, Font::HAlignLeft, Font::VAlignTop);
}

void Dialog::writeSubTitle(Surface& s, const std::string &subtitle)
{
	std::string wrapped = gmenu2x->font->wordWrap(subtitle, gmenu2x->resX - 48);
	gmenu2x->font->write(s, wrapped, 40,
			gmenu2x->skinConfInt["topBarHeight"]
				- gmenu2x->font->getTextHeight(wrapped),
			Font::HAlignLeft, Font::VAlignTop);
}
