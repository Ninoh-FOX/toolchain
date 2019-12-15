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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>
#include <unordered_map>

#include "inputmanager.h"

typedef std::unordered_map<std::string, std::string, std::hash<std::string>> ConfStrHash;
typedef std::unordered_map<std::string, int, std::hash<std::string>> ConfIntHash;

class case_less {
public:
	bool operator()(const std::string &left, const std::string &right) const;
};

inline bool isUTF8Starter(char c) {
	return (c & 0xC0) != 0x80;
}

/** Returns the string with whitespace stripped from both ends. */
std::string trim(const std::string& s);
/** Returns the string with whitespace stripped from the start. */
std::string ltrim(const std::string& s);
/** Returns the string with whitespace stripped from the end. */
std::string rtrim(const std::string& s);

/** Returns the contents of the given file as a string. */
std::string readFileAsString(const char *filename);

std::string strreplace(std::string orig, const std::string &search, const std::string &replace);
std::string cmdclean(std::string cmdline);

/**
 * Returns the parent directory of the given directory path, or "/" if there is
 * no parent.
 * This function does not check the file system: it is only string manipulation.
 * @return The parent directory path, including a trailing '/'.
 */
std::string parentDir(std::string const& dir);

inline std::string trimExtension(std::string const& filename) {
	return filename.substr(0, filename.rfind('.'));
}

bool fileExists(const std::string &file);
bool rmtree(std::string path);

int constrain(int x, int imin, int imax);

int evalIntConf(ConfIntHash& hash, const std::string &key, int def, int imin, int imax);

bool split(std::vector<std::string> &vec, const std::string &str,
		const std::string &delim, bool destructive=true);

int intTransition(int from, int to, long int tickStart, long duration=500,
		long tickNow=-1);

void inject_user_event(enum EventCode code = REPAINT_MENU,
			void *data1 = NULL, void *data2 = NULL);

#endif // UTILITIES_H
