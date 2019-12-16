/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo   *
 *   massimiliano.torromeo@gmail.com   *
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

#include "surfacecollection.h"
#include "surface.h"
#include "utilities.h"
#include "debug.h"
#include "gmenu2x.h"

#include <iostream>

using std::endl;
using std::string;

SurfaceCollection::SurfaceCollection()
	: skin("default")
{
}

SurfaceCollection::~SurfaceCollection() {}

void SurfaceCollection::setSkin(const string &skin) {
	this->skin = skin;
}

/* Returns the location of a skin directory,
 * from its name given as a parameter. */
string SurfaceCollection::getSkinPath(const string &skin)
{
	string path = GMENU2X_SYSTEM_DIR "/skins/" + skin;
	if (fileExists(path))
	  return path;

	return "";
}

string SurfaceCollection::getSkinFilePath(const string &file, bool useDefault)
{
	string path = GMENU2X_SYSTEM_DIR "/skins/" + skin + "/" + file;
	if (fileExists(path))
	  return path;

	/* If it is nowhere to be found, as a last resort we check the
	 * "Default" skin for a corresponding (but probably not similar) file. */
	if (useDefault) {
		path = GMENU2X_SYSTEM_DIR "/skins/Default/" + file;
		if (fileExists(path))
		  return path;
	}

	return "";
}

void SurfaceCollection::debug() {
	SurfaceHash::iterator end = surfaces.end();
	for(SurfaceHash::iterator curr = surfaces.begin(); curr != end; curr++){
		DEBUG("key: %s\n", curr->first.c_str());
	}
}

bool SurfaceCollection::exists(const string &path) {
	return surfaces.find(path) != surfaces.end();
}

OffscreenSurface *SurfaceCollection::add(const string &path) {
	if (path.empty()) return NULL;
	if (exists(path)) del(path);
	string filePath = path;

	if (filePath.substr(0,5)=="skin:") {
		filePath = getSkinFilePath(filePath.substr(5,filePath.length()));
		if (filePath.empty())
			return NULL;
	} else if ((filePath.find('#') == filePath.npos) && (!fileExists(filePath))) {
		WARNING("Unable to add image %s\n", path.c_str());
		return NULL;
	}

	DEBUG("Adding surface: '%s'\n", path.c_str());
	// TODO: Be safe.
	auto s = OffscreenSurface::loadImage(filePath).release();
	if (s) {
		surfaces[path] = s;
	}
	return s;
}

OffscreenSurface *SurfaceCollection::addSkinRes(const string &path, bool useDefault) {
	if (path.empty()) return NULL;
	if (exists(path)) del(path);

	string skinpath = getSkinFilePath(path, useDefault);
	if (skinpath.empty())
		return NULL;

	DEBUG("Adding skin surface: '%s'\n", path.c_str());
	// TODO: Be safe.
	auto s = OffscreenSurface::loadImage(skinpath).release();
	if (s) {
		surfaces[path] = s;
	}
	return s;
}

void SurfaceCollection::del(const string &path) {
	SurfaceHash::iterator i = surfaces.find(path);
	if (i != surfaces.end()) {
		delete i->second;
		surfaces.erase(i);
	}

	DEBUG("Unloading skin surface: '%s'\n", path.c_str());
}

void SurfaceCollection::clear() {
	surfaces.clear();
}

void SurfaceCollection::move(const string &from, const string &to) {
	del(to);
	surfaces[to] = surfaces[from];
	surfaces.erase(from);
}

OffscreenSurface *SurfaceCollection::operator[](const string &key) {
	SurfaceHash::iterator i = surfaces.find(key);
	if (i == surfaces.end())
		return add(key);
	else
		return i->second;
}

OffscreenSurface *SurfaceCollection::skinRes(const string &key, bool useDefault) {
	if (key.empty()) return NULL;

	SurfaceHash::iterator i = surfaces.find(key);
	if (i == surfaces.end())
		return addSkinRes(key, useDefault);
	else
		return i->second;
}
