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
#ifndef SURFACECOLLECTION_H
#define SURFACECOLLECTION_H

#include <string>
#include <unordered_map>

class OffscreenSurface;
class Surface;

typedef std::unordered_map<std::string, OffscreenSurface *> SurfaceHash;

/**
Hash Map of surfaces that loads surfaces not already loaded and reuses already loaded ones.

	@author Massimiliano Torromeo <massimiliano.torromeo@gmail.com>
*/
class SurfaceCollection {
public:
	SurfaceCollection();
	~SurfaceCollection();

	void setSkin(const std::string &skin);
	std::string getSkinFilePath(const std::string &file, bool useDefault = true);
	static std::string getSkinPath(const std::string &skin);

	void debug();

	OffscreenSurface *addSkinRes(const std::string &path, bool useDefault = true);
	void     del(const std::string &path);
	void     clear();
	void     move(const std::string &from, const std::string &to);
	bool     exists(const std::string &path);

	OffscreenSurface *operator[](const std::string &);
	OffscreenSurface *skinRes(const std::string &key, bool useDefault = true);

private:
	OffscreenSurface *add(const std::string &path);

	SurfaceHash surfaces;
	std::string skin;
};

#endif
