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

#include "utilities.h"

#include "debug.h"

#include <SDL.h>
#include <algorithm>

//for browsing the filesystem
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <strings.h>
#include <unistd.h>

using namespace std;

bool case_less::operator()(const string &left, const string &right) const {
	return strcasecmp(left.c_str(), right.c_str()) < 0;
}

string trim(const string& s) {
  auto b = s.find_first_not_of(" \t\r");
  auto e = s.find_last_not_of(" \t\r");
  return b == string::npos ? "" : string(s, b, e + 1 - b);
}

string ltrim(const string& s) {
  auto b = s.find_first_not_of(" \t\r");
  return b == string::npos ? "" : string(s, b);
}

string rtrim(const string& s) {
  auto e = s.find_last_not_of(" \t\r");
  return e == string::npos ? "" : string(s, 0, e + 1);
}

// See this article for a performance comparison of different approaches:
//   http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
string readFileAsString(const char *filename) {
	ifstream in(filename, ios::in | ios::binary);
	if (!in) {
		return "<error opening " + string(filename) + ">";
	}

	// Get file size.
	in.seekg(0, ios::end);
	auto size = max(int(in.tellg()), 0); // tellg() returns -1 on errors
	in.seekg(0, ios::beg);

	string contents(size, '\0');
	in.read(&contents[0], contents.size());
	in.close();

	if (in.fail()) {
		return "<error reading " + string(filename) + ">";
	} else {
		contents.shrink_to_fit();
		return contents;
	}
}

string parentDir(string const& dir) {
	// Note that size() is unsigned, so for short strings the '- 2' wraps
	// around and as a result the entire string is searched, which is fine.
	auto p = dir.rfind('/', dir.size() - 2);
	return p == string::npos ? "/" : dir.substr(0, p + 1);
}

bool fileExists(const string &file) {
	return access(file.c_str(), F_OK) == 0;
}

bool rmtree(string path) {
	DIR *dirp;
	struct stat st;
	struct dirent *dptr;
	string filepath;

	DEBUG("RMTREE: '%s'\n", path.c_str());

	if ((dirp = opendir(path.c_str())) == NULL) return false;
	if (path[path.length()-1]!='/') path += "/";

	while ((dptr = readdir(dirp))) {
		filepath = dptr->d_name;
		if (filepath=="." || filepath=="..") continue;
		filepath = path+filepath;
		int statRet = stat(filepath.c_str(), &st);
		if (statRet == -1) continue;
		if (S_ISDIR(st.st_mode)) {
			if (!rmtree(filepath)) return false;
		} else {
			if (unlink(filepath.c_str())!=0) return false;
		}
	}

	closedir(dirp);
	return rmdir(path.c_str())==0;
}

int constrain(int x, int imin, int imax) {
	return min(imax, max(imin, x));
}

//Configuration parsing utilities
int evalIntConf (ConfIntHash& hash, const std::string &key, int def, int imin, int imax) {
	auto it = hash.find(key);
	if (it == hash.end()) {
		return hash[key] = def;
	} else {
		return it->second = constrain(it->second, imin, imax);
	}
}

bool split (vector<string> &vec, const string &str, const string &delim, bool destructive) {
	vec.clear();

	if (delim.empty()) {
		vec.push_back(str);
		return false;
	}

	std::string::size_type i = 0;
	std::string::size_type j = 0;

	while(1) {
		j = str.find(delim,i);
		if (j==std::string::npos) {
			vec.push_back(str.substr(i));
			break;
		}

		if (!destructive)
			j += delim.size();

		vec.push_back(str.substr(i,j-i));

		if (destructive)
			i = j + delim.size();

		if (i==str.size()) {
			vec.push_back(std::string());
			break;
		}
	}

	return true;
}

string strreplace (string orig, const string &search, const string &replace) {
	string::size_type pos = orig.find( search, 0 );
	while (pos != string::npos) {
		orig.replace(pos,search.length(),replace);
		pos = orig.find( search, pos+replace.length() );
	}
	return orig;
}

string cmdclean (string cmdline) {
	string spchars = "\\`$();|{}&'\"*?<>[]!^~-#\n\r ";
	for (uint i=0; i<spchars.length(); i++) {
		string curchar = spchars.substr(i,1);
		cmdline = strreplace(cmdline, curchar, "\\"+curchar);
	}
	return cmdline;
}

int intTransition(int from, int to, long tickStart, long duration, long tickNow) {
	if (tickNow<0) tickNow = SDL_GetTicks();
	return constrain(((tickNow-tickStart) * (to-from)) / duration, from, to);
	//                    elapsed                 increments
}

void inject_user_event(enum EventCode code, void *data1, void *data2)
{
	SDL_UserEvent e = {
		.type = SDL_USEREVENT,
		.code = code,
		.data1 = data1,
		.data2 = data2,
	};

	/* Inject an user event, that will be handled as a "repaint"
	 * event by the InputManager */
	SDL_PushEvent((SDL_Event *) &e);
	DEBUG("Injecting event code %i\n", e.code);
}
