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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <unordered_map>

/**
Hash Map of translation strings.

	@author Massimiliano Torromeo <massimiliano.torromeo@gmail.com>
*/
class Translator {
private:
	std::string _lang;
	std::unordered_map<std::string, std::string> translations;

public:
	Translator(const std::string &lang="");
	~Translator();

	std::string lang();
	void setLang(const std::string &lang);
	bool exists(const std::string &term);
	std::string translate(const std::string &term,
			const char *replacestr = NULL, ...);
	std::string operator[](const std::string &term);
};

#endif // TRANSLATOR_H
