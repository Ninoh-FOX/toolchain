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

#include "linkapp.h"

#include "debug.h"
#include "gmenu2x.h"
#include "launcher.h"
#include "layer.h"
#include "menu.h"
#include "selector.h"
#include "surface.h"
#include "textmanualdialog.h"
#include "utilities.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <array>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <utility>

#ifdef HAVE_LIBOPK
#include <opk.h>
#endif

#ifdef HAVE_LIBXDGMIME
#include <xdgmime.h>
#endif

using namespace std;

static array<const char *, 4> tokens = { "%f", "%F", "%u", "%U", };


/**
 * Displays the launch message (loading screen).
 */
class LaunchLayer: public Layer {
public:
	LaunchLayer(LinkApp& app) : app(app) {}

	void paint(Surface &s) override {
		app.drawLaunch(s);
	}

	bool handleButtonPress(InputManager::Button) override {
		return true;
	}

	bool handleTouchscreen(Touchscreen&) override {
		return true;
	}

private:
	LinkApp& app;
};


#ifdef HAVE_LIBOPK
LinkApp::LinkApp(GMenu2X *gmenu2x_, string const& linkfile, bool deletable,
			struct OPK *opk, const char *metadata_)
#else
LinkApp::LinkApp(GMenu2X *gmenu2x_, string const& linkfile, bool deletable)
#endif
	: Link(gmenu2x_, bind(&LinkApp::start, this))
	, deletable(deletable)
{
	manual = "";
	file = linkfile;
#ifdef ENABLE_CPUFREQ
	setClock(gmenu2x->getDefaultAppClock());
#else
	setClock(0);
#endif
	selectordir = "";
	selectorfilter = "*";
	icon = iconPath = "";
	selectorbrowser = true;
	editable = true;
	edited = false;

	bool appTakesFileArg = true;
#ifdef HAVE_LIBOPK
	isOPK = !!opk;

	if (isOPK) {
		string::size_type pos;
		const char *key, *val;
		size_t lkey, lval;
		int ret;

		metadata.assign(metadata_);
		opkFile = file;
		pos = file.rfind('/');
		opkMount = file.substr(pos+1);
		pos = opkMount.rfind('.');
		opkMount = opkMount.substr(0, pos);

		appTakesFileArg = false;
		category = "applications";

		while ((ret = opk_read_pair(opk, &key, &lkey, &val, &lval))) {
			if (ret < 0) {
				ERROR("Unable to read meta-data\n");
				break;
			}

			char buf[lval + 1];
			sprintf(buf, "%.*s", lval, val);

			if (!strncmp(key, "Categories", lkey)) {
				category = buf;

				pos = category.find(';');
				if (pos != category.npos)
					category = category.substr(0, pos);
// NOTA ---> a partir de aquí se cargan los datos del OPK
			} else if ((!strncmp(key, "Name", lkey) && title.empty())
						|| !strncmp(key, ("Name[" + gmenu2x->tr["Lng"] +
								"]").c_str(), lkey)) {
				title = buf;

			} else if ((!strncmp(key, "Comment", lkey) && description.empty())
						|| !strncmp(key, ("Comment[" +
								gmenu2x->tr["Lng"] + "]").c_str(), lkey)) {
				description = buf;

			} else if (!strncmp(key, "Terminal", lkey)) {
				consoleApp = !strncmp(val, "true", lval);

			} else if (!strncmp(key, "X-OD-Manual", lkey)) {
				manual = buf;

			} else if (!strncmp(key, "Icon", lkey)) {
				/* Read the icon from the OPK only
				 * if it doesn't exist on the skin */
				this->icon = gmenu2x->sc.getSkinFilePath("icons/" + (string) buf + ".png");
				if (this->icon.empty()) {
					this->icon = linkfile + '#' + buf + ".png";
				}
				iconPath = this->icon;
				updateSurfaces();

			} else if (!strncmp(key, "Exec", lkey)) {
				string tmp = buf;
				opkExec=std::string("_")+buf;
				cutName(opkExec);

				for (auto token : tokens) {
					if (tmp.find(token) != tmp.npos) {
						selectordir = CARD_ROOT;
						appTakesFileArg = true;
						break;
					}
				}

				continue;
			}

#ifdef HAVE_LIBXDGMIME
			if (!strncmp(key, "MimeType", lkey)) {
				string mimetypes = buf;
				selectorfilter = "";

				while ((pos = mimetypes.find(';')) != mimetypes.npos) {
					int nb = 16;
					char *extensions[nb];
					string mimetype = mimetypes.substr(0, pos);
					mimetypes = mimetypes.substr(pos + 1);

					nb = xdg_mime_get_extensions_from_mime_type(
								mimetype.c_str(), extensions, nb);

					while (nb--) {
						selectorfilter += (string) extensions[nb] + ',';
						free(extensions[nb]);
					}
				}

				/* Remove last comma */
				if (!selectorfilter.empty()) {
					selectorfilter.erase(selectorfilter.end());
					DEBUG("Compatible extensions: %s\n", selectorfilter.c_str());
				}

				continue;
			}
#endif /* HAVE_LIBXDGMIME */
		}

		// if exist old config, rename it
		std::string fileold;
		fileold = gmenu2x->getHome() + "/sections/" + category + '/' + opkMount;
		file = gmenu2x->getHome() + "/sections/" + category + '/' + opkMount + opkExec;

		if(std::ifstream(fileold)) {
      rename(fileold.c_str(), file.c_str());
		}

		opkMount = (string) "/mnt/" + opkMount + '/';
		edited = true;
	}
#endif /* HAVE_LIBOPK */

	string line;
	ifstream infile (file.c_str(), ios_base::in);
	while (getline(infile, line, '\n')) {
		line = trim(line);
		if (line.empty()) continue;
		if (line[0]=='#') continue;

		string::size_type position = line.find("=");
		string name = trim(line.substr(0,position));
		string value = trim(line.substr(position+1));

// NOTA ---> a partir de aquí se cargan los datos de la aplicación que hay en .gmenu2x
		if (name == "clock") {
			setClock( atoi(value.c_str()) );
		} else if (name == "selectordir") {
			if (appTakesFileArg) setSelectorDir(value);
		} else if (name == "selectorbrowser") {
			if (value=="false") selectorbrowser = false;
// NOTA ---> estos datos sólo los carga si NO estamos viendo un OPK, peeeero, podemos forzarlo a que cargue datos
//           de .gmenu2x si exiten (o sea, que se hayan editado)
		} /*else if (!isOpk()) {        // para forzar a que cargue los datos hay que cambiar esta comprobación
                                   // lo suyo sería editar sólo: título, descripción e icono, lo demás no sé si tiene sentido
                                   // se puede hacer que estos 3 los cargue de .gmenu2x (si existen), y el resto que lo siga
                                   // haciendo desde el opk
                                   // EJEMPLO:
                                   // } else if(1) {          // forzamos a leer siempre los datos
                                   // if (name == "title") {
                                   //   if(title!="")         // si está editado, lo cargamos
                                   //     title=value;*/
			else if (name == "title") {
				title = value;
			} else if (name == "description") {
				description = value;
                                   // y estas líneas que se cargan siempre del opk, habría que añadir algo para que no se sobreescriban
                                   // con datos de .gmenu2x
                                   // } else if (name == "launchmsg" && !isOpk()) {
			} else if (name == "launchmsg" && !isOpk()) {
				launchMsg = value;
			} else if (name == "icon") {    // si es editable no hace falta meter !isOpk()
				setIcon(value);
			} else if (name == "exec" && !isOpk()) {
				exec = value;
			} else if (name == "params" && !isOpk()) {
				params = value;
			} else if (name == "manual") {
				manual = value;
			} else if (name == "consoleapp" && !isOpk()) {
				if (value == "true") consoleApp = true;
			} else if (name == "selectorfilter") {
				setSelectorFilter( value );
			} else if (name == "editable") {    // habría que añadir lo anterior en todos los ifs hasta este
				if (value == "false")
					editable = false;
			} else
				WARNING("Unrecognized option: '%s'\n", name.c_str());
	}
	infile.close();

	if (iconPath.empty()) searchIcon();
}

void LinkApp::loadIcon() {
	if (icon.compare(0, 5, "skin:") == 0) {
		string linkIcon = gmenu2x->sc.getSkinFilePath(
				icon.substr(5, string::npos));
		if (!fileExists(linkIcon))
			searchIcon();
		else
			setIconPath(linkIcon);

	} else if (!fileExists(icon)) {
		searchIcon();
	}
}

const string &LinkApp::searchIcon() {
	if (!iconPath.empty())
		return iconPath;

	string execicon = exec;
	string::size_type pos = exec.rfind(".");
	if (pos != string::npos) execicon = exec.substr(0,pos);
	execicon += ".png";
	string exectitle = execicon;
	pos = execicon.rfind("/");
	if (pos != string::npos)
		string exectitle = execicon.substr(pos+1,execicon.length());

	if (!gmenu2x->sc.getSkinFilePath("icons/"+exectitle).empty())
		iconPath = gmenu2x->sc.getSkinFilePath("icons/"+exectitle);
	else if (fileExists(execicon))
		iconPath = execicon;
	else
		iconPath = gmenu2x->sc.getSkinFilePath("icons/generic.png");

	return iconPath;
}

int LinkApp::clock() {
	return iclock;
}

const string &LinkApp::clockStr(int maxClock) {
	if (iclock>maxClock) setClock(maxClock);
	return sclock;
}

void LinkApp::setClock(int mhz) {
	iclock = mhz;
	stringstream ss;
	sclock = "";
	ss << iclock << "MHz";
	ss >> sclock;

	edited = true;
}

bool LinkApp::targetExists()
{
	return fileExists(exec);
}

bool LinkApp::save() {
	if (!edited) return false;
// NOTA ---> esto es lo más importante, los datos editados que se graban en .gmenu2x
	std::ostringstream out;
	if (!isOpk()) {   // si es un opk no guarda estos, pero si sacamos los que nos interesan, se guardan en .gmenu2x
		  // si queremos guardar este, lo llevamos...

		if (!launchMsg.empty()   ) out << "launchmsg="       << launchMsg       << endl;
		if (!exec.empty()        ) out << "exec="            << exec            << endl;
		if (!params.empty()      ) out << "params="          << params          << endl;
		if (consoleApp           ) out << "consoleapp=true"                     << endl;

	}
	// if (!title.empty()       ) out << "title="           << title           << endl;     // hasta aquí... si ponemos la línea aquí,
    if (!title.empty()       ) out << "title="           << title           << endl; // se guarda el nombre editado
	if (!description.empty() ) out << "description="     << description     << endl;
	if (!manual.empty()      ) out << "manual="          << manual          << endl;
	if (!icon.empty()        ) out << "icon="            << icon            << endl;
	if (selectorfilter != "*") out << "selectorfilter="  << selectorfilter  << endl;
	if (iclock != 0              ) out << "clock="           << iclock          << endl;
	if (!selectordir.empty()     ) out << "selectordir="     << selectordir     << endl;
	if (!selectorbrowser         ) out << "selectorbrowser=false"               << endl;

	if (out.tellp() > 0) {
		DEBUG("Saving app settings: %s\n", file.c_str());
		ofstream f(file.c_str());
		if (f.is_open()) {
			f << out.str();
			f.close();
			sync();
			return true;
		} else {
			ERROR("Error while opening the file '%s' for write.\n", file.c_str());
			return false;
		}
	} else {
		DEBUG("Empty app settings: %s\n", file.c_str());
		return unlink(file.c_str()) == 0 || errno == ENOENT;
	}
}

void LinkApp::drawLaunch(Surface& s) {
	//Darkened background
	s.box(0, 0, gmenu2x->resX, gmenu2x->resY, 0,0,0,150);

	string text = getLaunchMsg().empty()
		? gmenu2x->tr.translate("Launching $1", getTitle().c_str(), nullptr)
		: gmenu2x->tr.translate(getLaunchMsg().c_str(), nullptr);

	int textW = gmenu2x->font->getTextWidth(text);
	int boxW = 62+textW;
	int halfBoxW = boxW/2;

	//outer box
	s.box(gmenu2x->halfX-2-halfBoxW, gmenu2x->halfY-23, halfBoxW*2+5, 47, gmenu2x->skinConfColors[COLOR_MESSAGE_BOX_BG]);
	//inner rectangle
	s.rectangle(gmenu2x->halfX-halfBoxW, gmenu2x->halfY-21, boxW, 42, gmenu2x->skinConfColors[COLOR_MESSAGE_BOX_BORDER]);

	int x = gmenu2x->halfX+10-halfBoxW;
	/*if (!getIcon().empty())
		gmenu2x->sc[getIcon()]->blit(gmenu2x->s,x,104);
	else
		gmenu2x->sc["icons/generic.png"]->blit(gmenu2x->s,x,104);*/
  if(iconSurface)
    iconSurface->blit(s, x, gmenu2x->halfY - 16);
	gmenu2x->font->write(s, text, x + 42, gmenu2x->halfY + 1, Font::HAlignLeft, Font::VAlignMiddle);
}

void LinkApp::start() {
	if (selectordir.empty()) {
		gmenu2x->queueLaunch(prepareLaunch(""), make_shared<LaunchLayer>(*this));
	} else {
		selector();
	}
}

void LinkApp::showManual() {
	if (manual.empty())
		return;

#ifdef HAVE_LIBOPK
	if (isOPK) {
		vector<string> readme;
		char *ptr;
		struct OPK *opk;
		int err;
		void *buf;
		size_t len;

		opk = opk_open(opkFile.c_str());
		if (!opk) {
			WARNING("Unable to open OPK to read manual\n");
			return;
		}

		err = opk_extract_file(opk, manual.c_str(), &buf, &len);
		if (err < 0) {
			WARNING("Unable to read manual from OPK\n");
			return;
		}
		opk_close(opk);

		ptr = (char *) buf;
		string str(ptr, len);
		free(buf);

		if (manual.substr(manual.size()-8,8)==".man.txt") {
			TextManualDialog tmd(gmenu2x, getTitle(), getIconPath(), str);
			tmd.exec();
		} else {
			TextDialog td(gmenu2x, getTitle(), "ReadMe", getIconPath(), str);
			td.exec();
		}
		return;
	}
#endif
	if (!fileExists(manual))
		return;

	// Png manuals
	if (manual.substr(manual.size()-8,8)==".man.png") {
#ifdef ENABLE_CPUFREQ
		//Raise the clock to speed-up the loading of the manual
		gmenu2x->setSafeMaxClock();
#endif

		auto pngman = OffscreenSurface::loadImage(manual);
		if (!pngman) {
			return;
		}
		auto bg = OffscreenSurface::loadImage(gmenu2x->confStr["wallpaper"]);
		if (!bg) {
			bg = OffscreenSurface::emptySurface(gmenu2x->s->width(), gmenu2x->s->height());
		}
		bg->convertToDisplayFormat();

		stringstream ss;
		string pageStatus;

		bool close = false, repaint = true;
		int page = 0, pagecount = pngman->width() / 320;

		ss << pagecount;
		string spagecount;
		ss >> spagecount;

#ifdef ENABLE_CPUFREQ
		//Lower the clock
		gmenu2x->setMenuClock();
#endif

		while (!close) {
			OutputSurface& s = *gmenu2x->s;

			if (repaint) {
				bg->blit(s, 0, 0);
				pngman->blit(s, -page*320, 0);

				gmenu2x->drawBottomBar(s);
				int x = 5;
				x = gmenu2x->drawButton(s, "left", "", x);
				x = gmenu2x->drawButton(s, "right", gmenu2x->tr["Change page"], x);
				x = gmenu2x->drawButton(s, "cancel", "", x);
				x = gmenu2x->drawButton(s, "start", gmenu2x->tr["Exit"], x);

				ss.clear();
				ss << page+1;
				ss >> pageStatus;
				pageStatus = gmenu2x->tr["Page"]+": "+pageStatus+"/"+spagecount;
				gmenu2x->font->write(s, pageStatus, 310, 230, Font::HAlignRight, Font::VAlignMiddle);

				s.flip();
				repaint = false;
			}

            switch(gmenu2x->input.waitForPressedButton()) {
				case InputManager::SETTINGS:
                case InputManager::CANCEL:
                    close = true;
                    break;
                case InputManager::LEFT:
                    if (page > 0) {
                        page--;
                        repaint = true;
                    }
                    break;
                case InputManager::RIGHT:
                    if (page < pagecount-1) {
                        page++;
                        repaint=true;
                    }
                    break;
                default:
                    break;
            }
        }
		return;
	}

	// Txt manuals
	if (manual.substr(manual.size()-8,8)==".man.txt") {
		string text(readFileAsString(manual.c_str()));
		TextManualDialog tmd(gmenu2x, getTitle(), getIconPath(), text);
		tmd.exec();
		return;
	}

	//Readmes
	string str, line;
	ifstream infile(manual.c_str(), ios_base::in);
	if (infile.is_open()) {
		while (getline(infile, line, '\n')) {
			str.append(line).append("\n");
		}
		infile.close();

		TextDialog td(gmenu2x, getTitle(), "ReadMe", getIconPath(), str);
		td.exec();
	}
}

void LinkApp::selector(int startSelection, const string &selectorDir) {
	//Run selector interface
	Selector sel(gmenu2x, *this, selectorDir);
	int selection = sel.exec(startSelection);
	if (selection!=-1) {
		const string &selectedDir = sel.getDir();
		if (!selectedDir.empty()) {
			selectordir = selectedDir;
		}
		gmenu2x->writeTmp(selection, selectedDir);
		gmenu2x->queueLaunch(
				prepareLaunch(selectedDir + sel.getFile()),
				make_shared<LaunchLayer>(*this));
	}
}

unique_ptr<Launcher> LinkApp::prepareLaunch(const string &selectedFile) {
	save();

	if (!isOpk()) {
		//Set correct working directory
		string::size_type pos = exec.rfind("/");
		if (pos != string::npos) {
			string wd = exec.substr(0, pos + 1);
			chdir(wd.c_str());
			exec = wd + exec.substr(pos + 1);
			DEBUG("Changed working directory to %s\n", wd.c_str());
		}
	}

	if (!selectedFile.empty()) {
		string path = selectedFile;
		if (!isOpk())
			path = cmdclean(path);

		if (params.empty()) {
			params = path;
		} else {
			string::size_type pos;

			for (auto token : tokens) {
				while ((pos = params.find(token)) != params.npos) {
					params.replace(pos, 2, path);
				}
			}
		}
	}

	if (gmenu2x->confInt["outputLogs"] && !consoleApp) {
		int fd = open(LOG_FILE, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if (fd < 0) {
			ERROR("Unable to open log file for write: %s\n", LOG_FILE);
		} else {
			fflush(stdout);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			close(fd);
		}
	}

	gmenu2x->saveSelection();

	if (selectedFile.empty()) {
		gmenu2x->writeTmp();
	}
#ifdef ENABLE_CPUFREQ
	if (clock() != gmenu2x->confInt["menuClock"]) {
		gmenu2x->setClock(clock());
	}
#endif

	vector<string> commandLine;
	if (isOpk()) {
#ifdef HAVE_LIBOPK
		commandLine = { "opkrun", "-m", metadata, opkFile };
		if (!params.empty()) {
			commandLine.push_back(params);
		}
#endif
	} else {
		commandLine = { "/bin/sh", "-c", exec + " " + params };
	}

	return std::unique_ptr<Launcher>(new Launcher(
			move(commandLine), consoleApp));
}

const string &LinkApp::getExec() {
	return exec;
}

void LinkApp::setExec(const string &exec) {
	this->exec = exec;
	edited = true;
}

const string &LinkApp::getParams() {
	return params;
}

void LinkApp::setParams(const string &params) {
	this->params = params;
	edited = true;
}

const string &LinkApp::getManual() {
	return manual;
}

void LinkApp::setManual(const string &manual) {
	this->manual = manual;
	edited = true;
}

const string &LinkApp::getSelectorDir() {
	return selectordir;
}

void LinkApp::setSelectorDir(const string &selectordir) {
	this->selectordir = selectordir;
	if (!selectordir.empty() && selectordir[selectordir.length() - 1] != '/') {
		this->selectordir += "/";
	}
	edited = true;
}

bool LinkApp::getSelectorBrowser() {
	return selectorbrowser;
}

void LinkApp::setSelectorBrowser(bool value) {
	selectorbrowser = value;
	edited = true;
}

const string &LinkApp::getSelectorFilter() {
	return selectorfilter;
}

void LinkApp::setSelectorFilter(const string &selectorfilter) {
	this->selectorfilter = selectorfilter;
	edited = true;
}

void LinkApp::renameFile(const string &name) {
	file = name;
}

void LinkApp::cutName(std::string &name) {
  std::string tmp="";
  for(int f=0; f<name.size(); f++) {
    if(name[f]>=35 && name[f]<127 && name[f]!=37 && name[f]!=39 && name[f]!=42)
      tmp.append(name.substr(f,1));
  }
  name=tmp;
}
