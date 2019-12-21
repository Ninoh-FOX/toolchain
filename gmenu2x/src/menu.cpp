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

#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <unistd.h>
#include <ini.h>
#include <cassert>

#ifdef HAVE_LIBOPK
#include <opk.h>
#endif

#include "gmenu2x.h"
#include "linkapp.h"
#include "menu.h"
#include "monitor.h"
#include "filelister.h"
#include "utilities.h"
#include "debug.h"

using namespace std;


Menu::Animation::Animation()
	: curr(0)
{
}

void Menu::Animation::adjust(int delta)
{
	curr += delta;
}

void Menu::Animation::step()
{
	if (curr == 0) {
		ERROR("Computing step past animation end\n");
	} else if (curr < 0) {
		const int v = ((1 << 16) - curr) / 32;
		curr = std::min(0, curr + v);
	} else {
		const int v = ((1 << 16) + curr) / 32;
		curr = std::max(0, curr - v);
	}
}

Menu::Menu(GMenu2X *gmenu2x, Touchscreen &ts)
	: gmenu2x(gmenu2x)
	, ts(ts)
	, btnContextMenu(gmenu2x, ts, "skin:imgs/menu.png", "",
			std::bind(&GMenu2X::showContextMenu, gmenu2x))
{
	readSections(GMENU2X_SYSTEM_DIR "/sections");

	sort(sections.begin(),sections.end(),case_less());
	setSectionIndex(0);
	readLinks();

#ifdef HAVE_LIBOPK
	{
		struct dirent *dptr;
		DIR *dirp = opendir(CARD_ROOT);
		if (dirp) {
			while ((dptr = readdir(dirp))) {
				if (dptr->d_type != DT_DIR)
					continue;

				if (!strcmp(dptr->d_name, ".") || !strcmp(dptr->d_name, ".."))
					continue;

				openPackagesFromDir((string) CARD_ROOT + "/" +
							dptr->d_name + "/apps");
			}
			closedir(dirp);
		}
	}
#endif

	btnContextMenu.setPosition(gmenu2x->resX - 38, gmenu2x->bottomBarIconY);
}

Menu::~Menu() {
	freeLinks();
}

void Menu::readSections(std::string parentDir)
{
	DIR *dirp;
	struct dirent *dptr;

	dirp = opendir(parentDir.c_str());
	if (!dirp) return;

	while ((dptr = readdir(dirp))) {
		if (dptr->d_name[0] == '.' || dptr->d_type != DT_DIR)
			continue;

		if (find(sections.begin(), sections.end(), dptr->d_name) == sections.end()) {
			sections.emplace_back(dptr->d_name);
			vector<Link*> ll;
			links.push_back(ll);
		}
	}

	closedir(dirp);
}

void Menu::skinUpdated() {
	ConfIntHash &skinConfInt = gmenu2x->skinConfInt;

	//recalculate some coordinates based on the new element sizes
	linkColumns = (gmenu2x->resX - 10) / skinConfInt["linkWidth"];
	linkRows = (gmenu2x->resY - 35 - skinConfInt["topBarHeight"]) / skinConfInt["linkHeight"];

	//reload section icons
	vector<string>::size_type i = 0;
	for (string sectionName : sections) {
		gmenu2x->sc["skin:sections/" + sectionName + ".png"];

		for (Link *&link : links[i]) {
			link->loadIcon();
		}

		i++;
	}
}

void Menu::calcSectionRange(int &leftSection, int &rightSection) {
	ConfIntHash &skinConfInt = gmenu2x->skinConfInt;
	const int linkWidth = skinConfInt["linkWidth"];
	const int screenWidth = gmenu2x->resX;
	const int numSections = sections.size();
	rightSection = min(
			max(1, (screenWidth - 20 - linkWidth) / (2 * linkWidth)),
			numSections / 2);
	leftSection = max(
			-rightSection,
			rightSection - numSections + 1);
}

bool Menu::runAnimations() {
	if (sectionAnimation.isRunning()) {
		sectionAnimation.step();
	}
	return sectionAnimation.isRunning();
}

void Menu::paint(Surface &s) {
	const uint width = s.width(), height = s.height();
	Font &font = *gmenu2x->font;
	SurfaceCollection &sc = gmenu2x->sc;

	ConfIntHash &skinConfInt = gmenu2x->skinConfInt;
	const int topBarHeight = skinConfInt["topBarHeight"];
	const int bottomBarHeight = skinConfInt["bottomBarHeight"];
	const int linkWidth = skinConfInt["linkWidth"];
	const int linkHeight = skinConfInt["linkHeight"];
	RGBAColor &selectionBgColor = gmenu2x->skinConfColors[COLOR_SELECTION_BG];

	// Apply section header animation.
	int leftSection, rightSection;
	calcSectionRange(leftSection, rightSection);
	int sectionFP = sectionAnimation.currentValue();
	int sectionDelta = (sectionFP * linkWidth + (1 << 15)) >> 16;
	int centerSection = iSection - sectionDelta / linkWidth;
	sectionDelta %= linkWidth;
	if (sectionDelta < 0) {
		rightSection++;
	} else if (sectionDelta > 0) {
		leftSection--;
	}

	// Paint section headers.
	s.box(width / 2  - linkWidth / 2, 0, linkWidth, topBarHeight, selectionBgColor);
	const uint sectionLinkPadding = (topBarHeight - 32 - font.getLineSpacing()) / 3;
	const uint numSections = sections.size();
	for (int i = leftSection; i <= rightSection; i++) {
		uint j = (centerSection + numSections + i) % numSections;
		string sectionIcon = "skin:sections/" + sections[j] + ".png";
		Surface *icon = sc.exists(sectionIcon)
				? sc[sectionIcon]
				: sc.skinRes("icons/section.png");
		int x = width / 2 + i * linkWidth + sectionDelta;
		if (i == leftSection) {
			int t = sectionDelta > 0 ? linkWidth - sectionDelta : -sectionDelta;
			x -= (((t * t) / linkWidth) * t) / linkWidth;
		} else if (i == rightSection) {
			int t = sectionDelta < 0 ? sectionDelta + linkWidth : sectionDelta;
			x += (((t * t) / linkWidth) * t) / linkWidth;
		}
		icon->blit(s, x - 16, sectionLinkPadding, 32, 32);
		font.write(s, sections[j], x, topBarHeight - sectionLinkPadding,
				Font::HAlignCenter, Font::VAlignBottom);
	}
	sc.skinRes("imgs/section-l.png")->blit(s, 0, 0);
	sc.skinRes("imgs/section-r.png")->blit(s, width - 10, 0);

	vector<Link*> &sectionLinks = links[iSection];
	const uint numLinks = sectionLinks.size();
	gmenu2x->drawScrollBar(
			linkRows, (numLinks + linkColumns - 1) / linkColumns, iFirstDispRow);

	//Links
	const uint linksPerPage = linkColumns * linkRows;
	const int linkSpacingX = (width - 10 - linkColumns * linkWidth) / linkColumns;
	const int linkMarginX = (
			width - linkWidth * linkColumns - linkSpacingX * (linkColumns - 1)
			) / 2;
	const int linkSpacingY = (height - 35 - topBarHeight - linkRows * linkHeight) / linkRows;
	for (uint i = iFirstDispRow * linkColumns; i < iFirstDispRow * linkColumns + linksPerPage && i < numLinks; i++) {
		const int ir = i - iFirstDispRow * linkColumns;
		const int x = linkMarginX + (ir % linkColumns) * (linkWidth + linkSpacingX);
		const int y = ir / linkColumns * (linkHeight + linkSpacingY) + topBarHeight + 2;
		sectionLinks.at(i)->setPosition(x, y);

		if (i == (uint)iLink) {
			sectionLinks.at(i)->paintHover();
		}

		sectionLinks.at(i)->paint();
	}

	if (selLink()) {
		font.write(s, selLink()->getDescription(),
				width / 2, height - bottomBarHeight + 2,
				Font::HAlignCenter, Font::VAlignBottom);
	}

	LinkApp *linkApp = selLinkApp();
	if (linkApp) {
#ifdef ENABLE_CPUFREQ
    // show cpu icon and frequency
		auto cpu = OffscreenSurface::loadImage(
				sc.getSkinFilePath("imgs/cpu.png"));
		if (cpu) cpu->blit(s, gmenu2x->cpuX-20, gmenu2x->bottomBarIconY);
		font.write(s, linkApp->clockStr(gmenu2x->confInt["maxClock"]),
				gmenu2x->cpuX, gmenu2x->bottomBarTextY,
				Font::HAlignLeft, Font::VAlignMiddle);
#endif
		//Manual indicator
		if (!linkApp->getManual().empty())
			sc.skinRes("imgs/manual.png")->blit(
					s, gmenu2x->manualX, gmenu2x->bottomBarIconY);
	}

	if (ts.available()) {
		btnContextMenu.paint(s);
	}
}

bool Menu::handleButtonPress(InputManager::Button button) {
	switch (button) {
		case InputManager::ACCEPT:
			if (selLink() != NULL) selLink()->run();
			return true;
		case InputManager::UP:
			linkUp();
			return true;
		case InputManager::DOWN:
			linkDown();
			return true;
		case InputManager::LEFT:
			linkLeft();
			return true;
		case InputManager::RIGHT:
			linkRight();
			return true;
		case InputManager::ALTLEFT:
			decSectionIndex();
			return true;
		case InputManager::ALTRIGHT:
			incSectionIndex();
			return true;
		case InputManager::MENU:
			gmenu2x->showContextMenu();
			return true;
		default:
			return false;
	}
}

bool Menu::handleTouchscreen(Touchscreen &ts) {
	btnContextMenu.handleTS();

	ConfIntHash &skinConfInt = gmenu2x->skinConfInt;
	const int topBarHeight = skinConfInt["topBarHeight"];
	const int screenWidth = gmenu2x->resX;

	if (ts.pressed() && ts.getY() < topBarHeight) {
		int leftSection, rightSection;
		calcSectionRange(leftSection, rightSection);

		const int linkWidth = skinConfInt["linkWidth"];
		const int leftSectionX = screenWidth / 2 + leftSection * linkWidth;
		const int i = min(
				leftSection + max((ts.getX() - leftSectionX) / linkWidth, 0),
				rightSection);
		const uint numSections = sections.size();
		setSectionIndex((iSection + numSections + i) % numSections);

		ts.setHandled();
		return true;
	}

	const uint linksPerPage = linkColumns * linkRows;
	uint i = iFirstDispRow * linkColumns;
	while (i < (iFirstDispRow * linkColumns) + linksPerPage && i < sectionLinks()->size()) {
		if (sectionLinks()->at(i)->isPressed()) {
			setLinkIndex(i);
		}
		if (sectionLinks()->at(i)->handleTS()) {
			i = sectionLinks()->size();
		}
		i++;
	}
	return ts.handled();
}

/*====================================
   SECTION MANAGEMENT
  ====================================*/
void Menu::freeLinks() {
	for (vector< vector<Link*> >::iterator section = links.begin(); section<links.end(); section++)
		for (vector<Link*>::iterator link = section->begin(); link<section->end(); link++)
			delete *link;
}

vector<Link*> *Menu::sectionLinks(int i) {
	if (i<0 || i>(int)links.size())
		i = selSectionIndex();

	if (i<0 || i>(int)links.size())
		return NULL;

	return &links[i];
}

void Menu::decSectionIndex() {
	sectionAnimation.adjust(-1 << 16);
	setSectionIndex(iSection - 1);
}

void Menu::incSectionIndex() {
	sectionAnimation.adjust(1 << 16);
	setSectionIndex(iSection + 1);
}

int Menu::selSectionIndex() {
	return iSection;
}

const string &Menu::selSection() {
	return sections[iSection];
}

void Menu::setSectionIndex(int i) {
	if (i<0)
		i=sections.size()-1;
	else if (i>=(int)sections.size())
		i=0;
	iSection = i;

	iLink = 0;
	iFirstDispRow = 0;
}

/*====================================
   LINKS MANAGEMENT
  ====================================*/
void Menu::addActionLink(uint section, const string &title, Action action, const string &description, const string &icon) {
	assert(section < sections.size());

	Link *link = new Link(gmenu2x, action);
	link->setSize(gmenu2x->skinConfInt["linkWidth"], gmenu2x->skinConfInt["linkHeight"]);
	link->setTitle(title);
	link->setDescription(description);
	if (gmenu2x->sc.exists(icon)
			|| (icon.substr(0,5)=="skin:"
				&& !gmenu2x->sc.getSkinFilePath(icon.substr(5,icon.length())).empty())
			|| fileExists(icon)) {
		link->setIcon(icon);
	}

	links[section].push_back(link);
}

bool Menu::addLink(string path, string file, string section) {
	if (section.empty()) {
		section = selSection();
	} else if (find(sections.begin(),sections.end(),section)==sections.end()) {
		//section directory doesn't exists
		if (!addSection(section))
			return false;
	}

	//strip the extension from the filename
	string title = file;
	string::size_type pos = title.rfind(".");
	if (pos!=string::npos && pos>0) {
		string ext = title.substr(pos, title.length());
		transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		title = title.substr(0, pos);
	}

	string linkpath = GMenu2X::getHome() + "/sections";
	if (!fileExists(linkpath))
		mkdir(linkpath.c_str(), 0755);

	linkpath = GMenu2X::getHome() + "/sections/" + section;
	if (!fileExists(linkpath))
		mkdir(linkpath.c_str(), 0755);

	linkpath += "/" + title;
	int x=2;
	while (fileExists(linkpath)) {
		stringstream ss;
		linkpath = "";
		ss << x;
		ss >> linkpath;
		linkpath = GMenu2X::getHome()+"/sections/"+section+"/"+title+linkpath;
		x++;
	}

	INFO("Adding link: '%s'\n", linkpath.c_str());

	if (path[path.length()-1]!='/') path += "/";
	//search for a manual
	pos = file.rfind(".");
	string exename = path+file.substr(0,pos);
	string manual = "";
	if (fileExists(exename+".man.png")) {
		manual = exename+".man.png";
	} else if (fileExists(exename+".man.txt")) {
		manual = exename+".man.txt";
	} else {
		//scan directory for a file like *readme*
		FileLister fl;
		fl.setShowDirectories(false);
		fl.setFilter(".txt");
		fl.browse(path);
		bool found = false;
		for (uint x=0; x<fl.size() && !found; x++) {
			string lcfilename = fl[x];

			if (lcfilename.find("readme") != string::npos) {
				found = true;
				manual = path+fl.getFiles()[x];
			}
		}
	}

	INFO("Manual: '%s'\n", manual.c_str());

	string shorttitle=title, description="", exec=path+file, icon="";
	if (fileExists(exename+".png")) icon = exename+".png";

	//Reduce title lenght to fit the link width
	if (gmenu2x->font->getTextWidth(shorttitle)>gmenu2x->skinConfInt["linkWidth"]) {
		while (gmenu2x->font->getTextWidth(shorttitle+"..")>gmenu2x->skinConfInt["linkWidth"])
			shorttitle = shorttitle.substr(0,shorttitle.length()-1);
		shorttitle += "..";
	}

	ofstream f(linkpath.c_str());
	if (f.is_open()) {
		f << "title=" << shorttitle << endl;
		f << "exec=" << exec << endl;
		if (!description.empty()) f << "description=" << description << endl;
		if (!icon.empty()) f << "icon=" << icon << endl;
		if (!manual.empty()) f << "manual=" << manual << endl;
		f.close();
 		sync();
		int isection = find(sections.begin(),sections.end(),section) - sections.begin();
		if (isection>=0 && isection<(int)sections.size()) {

			INFO("Section: '%s(%i)'\n", sections[isection].c_str(), isection);

			LinkApp* link = new LinkApp(gmenu2x, linkpath, true);
			link->setSize(gmenu2x->skinConfInt["linkWidth"],gmenu2x->skinConfInt["linkHeight"]);
			links[isection].push_back( link );
		}
	} else {

		ERROR("Error while opening the file '%s' for write.\n", linkpath.c_str());

		return false;
	}

	return true;
}

bool Menu::addSection(const string &sectionName) {
	string sectiondir = GMenu2X::getHome() + "/sections";
	if (!fileExists(sectiondir))
		mkdir(sectiondir.c_str(), 0755);

	sectiondir = sectiondir + "/" + sectionName;
	if (mkdir(sectiondir.c_str(), 0755) == 0) {
		sections.push_back(sectionName);
		vector<Link*> ll;
		links.push_back(ll);
		return true;
	}
	return false;
}

void Menu::deleteSelectedLink()
{
	bool icon_used = false;
	string iconpath = selLink()->getIconPath();

	INFO("Deleting link '%s'\n", selLink()->getTitle().c_str());

	if (selLinkApp()!=NULL)
		unlink(selLinkApp()->getFile().c_str());
	sectionLinks()->erase( sectionLinks()->begin() + selLinkIndex() );
	setLinkIndex(selLinkIndex());

	for (vector< vector<Link*> >::iterator section = links.begin();
				!icon_used && section<links.end(); section++)
		for (vector<Link*>::iterator link = section->begin();
					!icon_used && link<section->end(); link++)
			icon_used = iconpath == (*link)->getIconPath();

	if (!icon_used)
	  gmenu2x->sc.del(iconpath);
}

void Menu::deleteSelectedSection() {
	INFO("Deleting section '%s'\n", selSection().c_str());

	gmenu2x->sc.del("sections/"+selSection()+".png");
	links.erase( links.begin()+selSectionIndex() );
	sections.erase( sections.begin()+selSectionIndex() );
	setSectionIndex(0); //reload sections
}

bool Menu::linkChangeSection(uint linkIndex, uint oldSectionIndex, uint newSectionIndex) {
	if (oldSectionIndex<sections.size() && newSectionIndex<sections.size() && linkIndex<sectionLinks(oldSectionIndex)->size()) {
		sectionLinks(newSectionIndex)->push_back( sectionLinks(oldSectionIndex)->at(linkIndex) );
		sectionLinks(oldSectionIndex)->erase( sectionLinks(oldSectionIndex)->begin()+linkIndex );
		//Select the same link in the new position
		setSectionIndex(newSectionIndex);
		setLinkIndex(sectionLinks(newSectionIndex)->size()-1);
		return true;
	}
	return false;
}

void Menu::linkLeft() {
	if (iLink % linkColumns == 0)
		setLinkIndex(sectionLinks()->size() > iLink + linkColumns - 1
				? iLink + linkColumns - 1 : sectionLinks()->size() - 1);
	else
		setLinkIndex(iLink - 1);
}

void Menu::linkRight() {
	if (iLink % linkColumns == linkColumns - 1
			|| iLink == (int)sectionLinks()->size() - 1)
		setLinkIndex(iLink - iLink % linkColumns);
	else
		setLinkIndex(iLink + 1);
}

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

void Menu::linkUp() {
	int l = iLink - linkColumns;
	if (l < 0) {
		const auto numLinks = sectionLinks()->size();
		unsigned int rows = DIV_ROUND_UP(numLinks, linkColumns);
		l = (rows * linkColumns) + l;
		if (l >= static_cast<int>(numLinks))
			l -= linkColumns;
	}
	setLinkIndex(l);
}

void Menu::linkDown() {
	int l = iLink + linkColumns;
	const auto numLinks = sectionLinks()->size();
	if (l >= static_cast<int>(numLinks)) {
		unsigned int rows = DIV_ROUND_UP(numLinks, linkColumns);
		unsigned int curCol = DIV_ROUND_UP(iLink + 1, linkColumns);
		if (rows > curCol)
			l = numLinks - 1;
		else
			l %= linkColumns;
	}
	setLinkIndex(l);
}

int Menu::selLinkIndex() {
	return iLink;
}

Link *Menu::selLink() {
	if (sectionLinks()->size()==0) return NULL;
	return sectionLinks()->at(iLink);
}

LinkApp *Menu::selLinkApp() {
	return dynamic_cast<LinkApp*>(selLink());
}

void Menu::setLinkIndex(int i) {
	const int numLinks = static_cast<int>(sectionLinks()->size());
	if (i < 0)
		i = numLinks - 1;
	else if (i >= numLinks)
		i = 0;
	iLink = i;

	int row = i / linkColumns;
	if (row >= (int)(iFirstDispRow + linkRows - 1))
		iFirstDispRow = min(row + 1, (int)DIV_ROUND_UP(numLinks, linkColumns) - 1)
						- linkRows + 1;
	else if (row <= (int)iFirstDispRow)
		iFirstDispRow = max(row - 1, 0);
}

#ifdef HAVE_LIBOPK
void Menu::openPackagesFromDir(std::string path)
{
	DEBUG("Opening packages from directory: %s\n", path.c_str());
	readPackages(path);
#ifdef ENABLE_INOTIFY
	monitors.emplace_back(new Monitor(path.c_str()));
#endif
}

void Menu::openPackage(std::string path, bool order)
{
	/* First try to remove existing links of the same OPK
	 * (needed for instance when an OPK is modified) */
	removePackageLink(path);

	struct OPK *opk = opk_open(path.c_str());
	if (!opk) {
		ERROR("Unable to open OPK %s\n", path.c_str());
		return;
	}

	for (;;) {
		unsigned int i;
		bool has_metadata = false;
		const char *name;
		LinkApp *link;

		for (;;) {
			string::size_type pos;
			int ret = opk_open_metadata(opk, &name);
			if (ret < 0) {
				ERROR("Error while loading meta-data\n");
				break;
			} else if (!ret)
			  break;

			/* Strip .desktop */
			string metadata(name);
			pos = metadata.rfind('.');
			metadata = metadata.substr(0, pos);

			/* Keep only the platform name */
			pos = metadata.rfind('.');
			metadata = metadata.substr(pos + 1);

			if (metadata == PLATFORM || metadata == "all") {
				has_metadata = true;
				break;
			}
		}

		if (!has_metadata)
		  break;

		// Note: OPK links can only be deleted by removing the OPK itself,
		//       but that is not something we want to do in the menu,
		//       so consider this link undeletable.
		link = new LinkApp(gmenu2x, path, false, opk, name);
		link->setSize(gmenu2x->skinConfInt["linkWidth"], gmenu2x->skinConfInt["linkHeight"]);

		addSection(link->getCategory());
		for (i = 0; i < sections.size(); i++) {
			if (sections[i] == link->getCategory()) {
				links[i].push_back(link);
				break;
			}
		}
	}

	opk_close(opk);

	if (order)
		orderLinks();
}

void Menu::readPackages(std::string parentDir)
{
	DIR *dirp;
	struct dirent *dptr;
	vector<string> linkfiles;

	dirp = opendir(parentDir.c_str());
	if (!dirp)
		return;

	while ((dptr = readdir(dirp))) {
		char *c;

		if (dptr->d_type != DT_REG)
			continue;

		c = strrchr(dptr->d_name, '.');
		if (!c) /* File without extension */
			continue;

		if (strcasecmp(c + 1, "opk"))
			continue;

		if (dptr->d_name[0] == '.') {
			// Ignore hidden files.
			// Mac OS X places these on SD cards, probably to store metadata.
			continue;
		}

		openPackage(parentDir + '/' + dptr->d_name, false);
	}

	closedir(dirp);
	orderLinks();
}

#ifdef ENABLE_INOTIFY
/* Remove all links that correspond to the given path.
 * If "path" is a directory, it will remove all links that
 * correspond to an OPK present in the directory. */
void Menu::removePackageLink(std::string path)
{
	for (vector< vector<Link*> >::iterator section = links.begin();
				section < links.end(); section++) {
		for (vector<Link*>::iterator link = section->begin();
					link < section->end(); link++) {
			LinkApp *app = dynamic_cast<LinkApp *> (*link);
			if (!app || !app->isOpk() || app->getOpkFile().empty())
				continue;

			if (app->getOpkFile().compare(0, path.size(), path) == 0) {
				DEBUG("Removing link corresponding to package %s\n",
							app->getOpkFile().c_str());
				section->erase(link);
				if (section - links.begin() == iSection
							&& iLink == (int) section->size())
					setLinkIndex(iLink - 1);
				link--;
			}
		}
	}

	/* Remove registered monitors */
	for (auto it = monitors.begin(); it < monitors.end(); ++it) {
		if ((*it)->getPath().compare(0, path.size(), path) == 0) {
			monitors.erase(it);
		}
	}
}
#endif
#endif

static bool compare_links(Link *a, Link *b)
{
	LinkApp *app1 = dynamic_cast<LinkApp *>(a);
	LinkApp *app2 = dynamic_cast<LinkApp *>(b);
	bool app1_is_opk = app1 && app1->isOpk(),
		 app2_is_opk = app2 && app2->isOpk();

	if (app1_is_opk && !app2_is_opk)
			return false;
	if (app2_is_opk && !app1_is_opk)
			return true;
	return a->getTitle().compare(b->getTitle()) <= 0;
}

void Menu::orderLinks()
{
	for (auto& section : links) {
		sort(section.begin(), section.end(), compare_links);
	}
}

void Menu::readLinks()
{
	iLink = 0;
	iFirstDispRow = 0;

	for (uint i=0; i<links.size(); i++) {
		links[i].clear();

		int correct = (i>sections.size() ? iSection : i);
		string const& section = sections[correct];

		readLinksOfSection(GMENU2X_SYSTEM_DIR "/sections/" + section, i);
	}

	orderLinks();
}

void Menu::readLinksOfSection(std::string const& path, uint i)
{
	DIR *dirp = opendir(path.c_str());
	if (!dirp) return;

	// Check whether link files in this directory could be deleted.
	bool deletable = access(path.c_str(), W_OK) == 0;

	while (struct dirent *dptr = readdir(dirp)) {
		if (dptr->d_type != DT_REG) continue;
		string linkfile = path + '/' + dptr->d_name;

		LinkApp *link = new LinkApp(gmenu2x, linkfile, deletable);
		if (link->targetExists()) {
			link->setSize(
					gmenu2x->skinConfInt["linkWidth"],
					gmenu2x->skinConfInt["linkHeight"]);
			links[i].push_back(link);
		} else {
			delete link;
		}
	}

	closedir(dirp);
}

void Menu::renameSection(int index, const string &name) {
	sections[index] = name;
}
