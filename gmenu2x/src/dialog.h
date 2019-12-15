#ifndef __DIALOG_H__
#define __DIALOG_H__

#include <string>

class GMenu2X;
class Surface;

class Dialog
{
public:
	Dialog(GMenu2X *gmenu2x);

protected:
	void drawTitleIcon(Surface& s, const std::string &icon, bool skinRes = false);
	void writeTitle(Surface& s, const std::string &title);
	void writeSubTitle(Surface& s, const std::string &subtitle);

	GMenu2X *gmenu2x;
};

#endif
