#ifndef __BUTTONBOX_H__
#define __BUTTONBOX_H__

#include "iconbutton.h"

#include <memory>
#include <vector>

class GMenu2X;
class Surface;

class ButtonBox
{
public:
	void add(std::unique_ptr<IconButton> button);
	void clear();

	void paint(Surface& s, int x, int y);
	void handleTS();

private:
	std::vector<std::unique_ptr<IconButton>> buttons;
};

#endif
