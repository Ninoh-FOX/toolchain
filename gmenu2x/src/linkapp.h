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

#ifndef LINKAPP_H
#define LINKAPP_H

#include "link.h"

#include <memory>
#include <string>

class GMenu2X;
class Launcher;
class Surface;

/**
Parses links files.

	@author Massimiliano Torromeo <massimiliano.torromeo@gmail.com>
*/
class LinkApp : public Link {
private:
	std::string sclock;
	int iclock;
	std::string exec, params, workdir, manual, selectordir, selectorfilter;
	bool selectorbrowser, deletable, editable;

	std::string file;

	bool dontleave;
#ifdef HAVE_LIBOPK
	bool isOPK;
	std::string opkMount, opkFile, opkExec, category, metadata;
#endif

	void start();

protected:
	virtual const std::string &searchIcon();

public:
#ifdef HAVE_LIBOPK
	const std::string &getCategory() { return category; }
	bool isOpk() { return isOPK; }
	const std::string &getOpkFile() { return opkFile; }

	LinkApp(GMenu2X *gmenu2x, std::string const& linkfile, bool deletable,
				struct OPK *opk = NULL, const char *metadata = NULL);
#else
	LinkApp(GMenu2X *gmenu2x, std::string const& linkfile, bool deletable);
	bool isOpk() { return false; }
#endif

	virtual void loadIcon();

	bool consoleApp = false;

	const std::string &getExec();
	void setExec(const std::string &exec);
	const std::string &getParams();
	void setParams(const std::string &params);
	const std::string &getManual();
	void setManual(const std::string &manual);
	const std::string &getSelectorDir();
	void setSelectorDir(const std::string &selectordir);
	bool getSelectorBrowser();
	void setSelectorBrowser(bool value);
	const std::string &getSelectorFilter();
	void setSelectorFilter(const std::string &selectorfilter);

	int clock();
	const std::string &clockStr(int maxClock);
	void setClock(int mhz);

	bool save();
	void showManual();
	void selector(int startSelection=0, const std::string &selectorDir="");
	bool targetExists();
	bool isDeletable() { return deletable; }
	bool isEditable() { return editable; }

	const std::string &getFile() { return file; }
	void renameFile(const std::string &name);

private:
  void cutName(std::string &name);
	void drawLaunch(Surface& s);
	std::unique_ptr<Launcher> prepareLaunch(const std::string &selectedFile);

	friend class LaunchLayer;
};

#endif
