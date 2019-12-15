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

#ifndef MENU_H
#define MENU_H

#include "iconbutton.h"
#include "layer.h"
#include "link.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class GMenu2X;
class IconButton;
class LinkApp;
class Monitor;


/**
Handles the menu structure

	@author Massimiliano Torromeo <massimiliano.torromeo@gmail.com>
*/
class Menu : public Layer {
private:
	class Animation {
	public:
		Animation();
		bool isRunning() { return curr != 0; }
		int currentValue() { return curr; }
		void adjust(int delta);
		void step();
	private:
		int curr;
	};

	GMenu2X *gmenu2x;
	Touchscreen &ts;
	IconButton btnContextMenu;
	int iSection, iLink;
	uint iFirstDispRow;
	std::vector<std::string> sections;
	std::vector<std::vector<Link*>> links;

	uint linkColumns, linkRows;

	Animation sectionAnimation;

	/**
	 * Determine which section headers are visible.
	 * The output values are relative to the middle section at 0.
	 */
	void calcSectionRange(int &leftSection, int &rightSection);

	std::vector<Link*> *sectionLinks(int i = -1);

	void readLinks();
	void freeLinks();

	// Load all the sections of the given "sections" directory.
	void readSections(std::string parentDir);

#ifdef HAVE_LIBOPK
	// Load all the .opk packages of the given directory
	void readPackages(std::string parentDir);
#ifdef ENABLE_INOTIFY
	std::vector<std::unique_ptr<Monitor>> monitors;
#endif
#endif

	// Load all the links on the given section directory.
	void readLinksOfSection(std::string const& path, uint i);

	void decSectionIndex();
	void incSectionIndex();
	void linkLeft();
	void linkRight();
	void linkUp();
	void linkDown();

public:
	typedef std::function<void(void)> Action;

	Menu(GMenu2X *gmenu2x, Touchscreen &ts);
	virtual ~Menu();

#ifdef HAVE_LIBOPK
	void openPackage(std::string path, bool order = true);
	void openPackagesFromDir(std::string path);
#ifdef ENABLE_INOTIFY
	void removePackageLink(std::string path);
#endif
#endif

	int selSectionIndex();
	const std::string &selSection();
	void setSectionIndex(int i);

	void addActionLink(uint section, const std::string &title,
			Action action, const std::string &description="",
			const std::string &icon="");
	bool addLink(std::string path, std::string file, std::string section="");
	bool addSection(const std::string &sectionName);
	void deleteSelectedLink();
	void deleteSelectedSection();

	void skinUpdated();
	void orderLinks();

	// Layer implementation:
	virtual bool runAnimations();
	virtual void paint(Surface &s);
	virtual bool handleButtonPress(InputManager::Button button);
	virtual bool handleTouchscreen(Touchscreen &ts);

	bool linkChangeSection(uint linkIndex, uint oldSectionIndex, uint newSectionIndex);

	int selLinkIndex();
	Link *selLink();
	LinkApp *selLinkApp();
	void setLinkIndex(int i);

	const std::vector<std::string> &getSections() { return sections; }
	void renameSection(int index, const std::string &name);
};

#endif // MENU_H
