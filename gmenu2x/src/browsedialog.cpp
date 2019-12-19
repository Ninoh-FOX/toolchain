#include "browsedialog.h"

#include "filelister.h"
#include "gmenu2x.h"
#include "iconbutton.h"
#include "surface.h"
#include "utilities.h"

using std::bind;
using std::string;
using std::unique_ptr;

BrowseDialog::BrowseDialog(
		GMenu2X *gmenu2x, Touchscreen &ts_,
		const string &title, const string &subtitle)
	: Dialog(gmenu2x)
	, ts(ts_)
	, title(title)
	, subtitle(subtitle)
	, ts_pressed(false)
{
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/left.png")));
	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/cancel.png",
			gmenu2x->tr["Up one folder"],
			bind(&BrowseDialog::directoryUp, this))));

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/accept.png",
			gmenu2x->tr["Select"],
			bind(&BrowseDialog::directoryEnter, this))));

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/start.png",
			gmenu2x->tr["Confirm"],
			bind(&BrowseDialog::confirm, this))));

	buttonBox.add(unique_ptr<IconButton>(new IconButton(
			gmenu2x, ts, "skin:imgs/buttons/select.png",
			gmenu2x->tr["Exit"],
			bind(&BrowseDialog::quit, this))));

	iconGoUp = gmenu2x->sc.skinRes("imgs/go-up.png");
	iconFolder = gmenu2x->sc.skinRes("imgs/folder.png");
	iconFile = gmenu2x->sc.skinRes("imgs/file.png");
}

BrowseDialog::~BrowseDialog()
{
}

bool BrowseDialog::exec()
{
	string path = getPath();

	if (path.empty() || !fileExists(path))
		setPath(CARD_ROOT);
		//|| path.compare(0, strlen(CARD_ROOT), CARD_ROOT) != 0)   // this make always browse in root

	const int topBarHeight = gmenu2x->skinConfInt["topBarHeight"];
	rowHeight = gmenu2x->font->getLineSpacing() + 1; // gp2x=15+1 / pandora=19+1
	rowHeight = constrain(rowHeight, 20, 40);
	numRows = (gmenu2x->resY - topBarHeight - 20) / rowHeight;
	clipRect = (SDL_Rect) {
		0,
		static_cast<Sint16>(topBarHeight + 1),
		static_cast<Uint16>(gmenu2x->resX - 9),
		static_cast<Uint16>(gmenu2x->resY - topBarHeight - 25)
	};
	touchRect = (SDL_Rect) {
		2,
		static_cast<Sint16>(topBarHeight + 4),
		static_cast<Uint16>(gmenu2x->resX - 12),
		clipRect.h
	};

	selected = 0;
	close = false;
	while (!close) {
		if (ts.available()) ts.poll();

		paint();

		handleInput();
	}

	return result;
}

BrowseDialog::Action BrowseDialog::getAction(InputManager::Button button)
{
	switch (button) {
		case InputManager::MENU:
			return BrowseDialog::ACT_CLOSE;
		case InputManager::UP:
			return BrowseDialog::ACT_UP;
		case InputManager::DOWN:
			return BrowseDialog::ACT_DOWN;
		case InputManager::ALTLEFT:
			return BrowseDialog::ACT_SCROLLUP;
		case InputManager::ALTRIGHT:
			return BrowseDialog::ACT_SCROLLDOWN;
		case InputManager::LEFT:
		case InputManager::CANCEL:
			return BrowseDialog::ACT_GOUP;
		case InputManager::ACCEPT:
			return BrowseDialog::ACT_SELECT;
		case InputManager::SETTINGS:
			return BrowseDialog::ACT_CONFIRM;
		default:
			return BrowseDialog::ACT_NONE;
	}
}

void BrowseDialog::handleInput()
{
	InputManager::Button button = gmenu2x->input.waitForPressedButton();

	BrowseDialog::Action action;
	if (ts_pressed && !ts.pressed()) {
		action = BrowseDialog::ACT_SELECT;
		ts_pressed = false;
	} else {
		action = getAction(button);
	}

	if (ts.available() && ts.pressed() && !ts.inRect(touchRect)) {
		ts_pressed = false;
	}

	if (action == BrowseDialog::ACT_SELECT && fl[selected] == "..") {
		action = BrowseDialog::ACT_GOUP;
	}
	switch (action) {
	case BrowseDialog::ACT_CLOSE:
		quit();
		break;
	case BrowseDialog::ACT_UP:
		if (selected == 0)
			selected = fl.size() - 1;
		else
			selected -= 1;
		break;
	case BrowseDialog::ACT_SCROLLUP:
		if (selected <= numRows - 2)
			selected = 0;
		else
			selected -= numRows - 2;
		break;
	case BrowseDialog::ACT_DOWN:
		if (fl.size() - 1 <= selected)
			selected = 0;
		else
			selected += 1;
		break;
	case BrowseDialog::ACT_SCROLLDOWN:
		if (selected+(numRows-2)>=fl.size())
			selected = fl.size()-1;
		else
			selected += numRows-2;
		break;
	case BrowseDialog::ACT_GOUP:
		directoryUp();
		break;
	case BrowseDialog::ACT_SELECT:
		if (fl.isDirectory(selected)) {
			directoryEnter();
			break;
		}
		/* Falltrough */
	case BrowseDialog::ACT_CONFIRM:
		confirm();
		break;
	default:
		break;
	}

	buttonBox.handleTS();
}

#include <iostream>

void BrowseDialog::directoryUp()
{
	string path = getPath();
	string::size_type p = path.rfind("/");

	if (p == path.size() - 1) {
		p = path.rfind("/", p - 1);
	}

	if (p == string::npos || path.length() < 2 || path[0] != '/') {
		quit();
	} else {
		selected = 0;
		if(p==0)
      p++;
		setPath(path.substr(0, p));
	}
}

void BrowseDialog::directoryEnter()
{
	string path = getPath();
	if (path[path.size()-1] != '/') {
		path += "/";
	}

	setPath(path + fl[selected]);

	selected = 0;
}

void BrowseDialog::confirm()
{
	result = true;
	close = true;
}

void BrowseDialog::quit()
{
	result = false;
	close = true;
}

void BrowseDialog::paint()
{
	OutputSurface& s = *gmenu2x->s;

	unsigned int i, iY;
	unsigned int firstElement, lastElement;
	unsigned int offsetY;

	OffscreenSurface bg(*gmenu2x->bg);
	drawTitleIcon(bg, "icons/explorer.png", true);
	writeTitle(bg, title);
	writeSubTitle(bg, subtitle);
	buttonBox.paint(bg, 5, gmenu2x->resY - 1);

	bg.convertToDisplayFormat();
	bg.blit(s, 0, 0);

	// TODO(MtH): I have no idea what the right value of firstElement would be,
	//            but originally it was undefined and that is never a good idea.
	firstElement = 0;
	if (selected>firstElement+numRows - 1) {
		firstElement = selected-numRows + 1;
	} else if (selected < firstElement) {
		firstElement = selected;
	}

	//Selection
	const int topBarHeight = gmenu2x->skinConfInt["topBarHeight"];
	iY = topBarHeight + 1 + (selected - firstElement) * rowHeight;
	s.box(2, iY, gmenu2x->resX - 12, rowHeight - 1,
			gmenu2x->skinConfColors[COLOR_SELECTION_BG]);

	lastElement = firstElement + numRows;
	if (lastElement > fl.size())
		lastElement = fl.size();

	offsetY = topBarHeight + 1;

	//Files & Directories
	s.setClipRect(clipRect);
	for (i = firstElement; i < lastElement; i++) {
		Surface *icon;
		if (fl.isDirectory(i)) {
			if (fl[i] == "..") {
				icon = iconGoUp;
			} else {
				icon = iconFolder;
			}
		} else {
			icon = iconFile;
		}
		icon->blit(s, 5, offsetY);
		gmenu2x->font->write(s, fl[i], 24, offsetY + rowHeight / 2,
				Font::HAlignLeft, Font::VAlignMiddle);

		if (ts.available() && ts.pressed()
				&& ts.inRect(touchRect.x, offsetY + 3, touchRect.w, rowHeight)) {
			ts_pressed = true;
			selected = i;
		}

		offsetY += rowHeight;
	}
	s.clearClipRect();

	gmenu2x->drawScrollBar(numRows,fl.size(), firstElement);
	s.flip();
}
