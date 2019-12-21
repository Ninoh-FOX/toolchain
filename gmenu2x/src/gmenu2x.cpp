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

#include "gp2x.h"

#include "background.h"
#include "cpu.h"
#include "debug.h"
#include "filedialog.h"
#include "filelister.h"
#include "font.h"
#include "gmenu2x.h"
#include "helppopup.h"
#include "iconbutton.h"
#include "inputdialog.h"
#include "launcher.h"
#include "linkapp.h"
#include "mediamonitor.h"
#include "menu.h"
#include "menusettingbool.h"
#include "menusettingdir.h"
#include "menusettingfile.h"
#include "menusettingimage.h"
#include "menusettingint.h"
#include "menusettingmultistring.h"
#include "menusettingrgba.h"
#include "menusettingstring.h"
#include "messagebox.h"
#include "powersaver.h"
#include "settingsdialog.h"
#include "textdialog.h"
#include "wallpaperdialog.h"
#include "utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <SDL.h>
#include <signal.h>

#include <sys/statvfs.h>
#include <errno.h>

#include <sys/fcntl.h> //for battery

#ifdef PLATFORM_PANDORA
//#include <pnd_container.h>
//#include <pnd_conf.h>
//#include <pnd_discovery.h>
#endif

//for browsing the filesystem
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

//for soundcard
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <sys/mman.h>

using namespace std;

#ifndef DEFAULT_WALLPAPER_PATH
#define DEFAULT_WALLPAPER_PATH \
  GMENU2X_SYSTEM_DIR "/skins/Default/wallpapers/default.png"
#endif

#ifdef _CARD_ROOT
const char *CARD_ROOT = _CARD_ROOT;
#elif defined(PLATFORM_A320) || defined(PLATFORM_GCW0)
const char *CARD_ROOT = "/media";
#else
const char *CARD_ROOT = "/card";
#endif

static GMenu2X *app;
static string gmenu2x_home;

// Note: Keep this in sync with the enum!
static const char *colorNames[NUM_COLORS] = {
	"topBarBg",
	"bottomBarBg",
	"selectionBg",
	"messageBoxBg",
	"messageBoxBorder",
	"messageBoxSelection",
};

static enum color stringToColor(const string &name)
{
	for (unsigned int i = 0; i < NUM_COLORS; i++) {
		if (strcmp(colorNames[i], name.c_str()) == 0) {
			return (enum color)i;
		}
	}
	return (enum color)-1;
}

static const char *colorToString(enum color c)
{
	return colorNames[c];
}

static void quit_all(int err) {
	delete app;
	SDL_Quit();
	exit(err);
}

const string GMenu2X::getHome()
{
	return gmenu2x_home;
}

static void set_handler(int signal, void (*handler)(int))
{
	struct sigaction sig;
	sigaction(signal, NULL, &sig);
	sig.sa_handler = handler;
	sigaction(signal, &sig, NULL);
}

int main(int /*argc*/, char * /*argv*/[]) {
	INFO("---- GMenu2X starting ----\n");

	set_handler(SIGINT, &quit_all);
	set_handler(SIGSEGV, &quit_all);
	set_handler(SIGTERM, &quit_all);

	char *home = getenv("HOME");
	if (home == NULL) {
		ERROR("Unable to find gmenu2x home directory. The $HOME variable is not defined.\n");
		return 1;
	}

	gmenu2x_home = (string)home + (string)"/.gmenu2x";
	if (mkdir(gmenu2x_home.c_str(), 0770) < 0 && errno != EEXIST) {
		ERROR("Unable to create gmenu2x home directory.\n");
		return 1;
	}

	DEBUG("Home path: %s.\n", gmenu2x_home.c_str());

	GMenu2X::run();

	return EXIT_FAILURE;
}

void GMenu2X::run() {
	auto menu = new GMenu2X();
	app = menu;
	DEBUG("Starting main()\n");
	menu->mainLoop();

	app = nullptr;
	Launcher *toLaunch = menu->toLaunch.release();
	delete menu;

	SDL_Quit();
	unsetenv("SDL_FBCON_DONT_CLEAR");

	if (toLaunch) {
		toLaunch->exec();
		// If control gets here, execution failed. Since we already destructed
		// everything, the easiest solution is to exit and let the system
		// respawn the menu.
		delete toLaunch;
	}
}

#ifdef ENABLE_CPUFREQ
void GMenu2X::initCPULimits() {
	// Note: These values are for the Dingoo.
	//       The NanoNote does not have cpufreq enabled in its kernel and
	//       other devices are not actively maintained.
	// TODO: Read min and max from sysfs.
	cpuFreqMin = 360;     // para RG350, antes 300
	cpuFreqMax = 1080;    // para RG350, antes 1100
	cpuFreqSafeMax = 1080;
	cpuFreqMenuDefault = 360;   // para RG350, antes 200
	cpuFreqAppDefault = 1080;
	cpuFreqMultiple = 20;

	// Round min and max values to the specified multiple.
	cpuFreqMin = ((cpuFreqMin + cpuFreqMultiple - 1) / cpuFreqMultiple)
			* cpuFreqMultiple;
	cpuFreqMax = (cpuFreqMax / cpuFreqMultiple) * cpuFreqMultiple;
	cpuFreqSafeMax = (cpuFreqSafeMax / cpuFreqMultiple) * cpuFreqMultiple;
	cpuFreqMenuDefault = (cpuFreqMenuDefault / cpuFreqMultiple) * cpuFreqMultiple;
	cpuFreqAppDefault = (cpuFreqAppDefault / cpuFreqMultiple) * cpuFreqMultiple;
}
#endif

GMenu2X::GMenu2X()
	: input(powerSaver)
{
	usbnet = samba = inet = web = false;
	useSelectionPng = false;

#ifdef ENABLE_CPUFREQ
	initCPULimits();
#endif
	//load config data
	readConfig();

	halfX = resX/2;
	halfY = resY/2;
	bottomBarIconY = resY-18;
	bottomBarTextY = resY-10;

	/* Do not clear the screen on exit.
	 * This may require an SDL patch available at
	 * https://github.com/mthuurne/opendingux-buildroot/blob
	 * 			/opendingux-2010.11/package/sdl/sdl-fbcon-clear-onexit.patch
	 */
	setenv("SDL_FBCON_DONT_CLEAR", "1", 0);

	if( SDL_Init(SDL_INIT_TIMER) < 0) {
		ERROR("Could not initialize SDL: %s\n", SDL_GetError());
		// TODO: We don't use exceptions, so don't put things that can fail
		//       in a constructor.
		exit(EXIT_FAILURE);
	}

	bg = NULL;
	font = NULL;
	setSkin(confStr["skin"], !fileExists(confStr["wallpaper"]));
	layers.insert(layers.begin(), make_shared<Background>(*this));

	/* We enable video at a later stage, so that the menu elements are
	 * loaded before SDL inits the video; this is made so that we won't show
	 * a black screen for a couple of seconds. */
	if( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		ERROR("Could not initialize SDL: %s\n", SDL_GetError());
		// TODO: We don't use exceptions, so don't put things that can fail
		//       in a constructor.
		exit(EXIT_FAILURE);
	}

	s = OutputSurface::open(resX, resY, confInt["videoBpp"]);

	if (!fileExists(confStr["wallpaper"])) {
		DEBUG("No wallpaper defined; we will take the default one.\n");
		confStr["wallpaper"] = DEFAULT_WALLPAPER_PATH;
	}

	initBG();

	/* the menu may take a while to load, so we show the background here */
	for (auto layer : layers)
		layer->paint(*s);
	s->flip();

	initMenu();

#ifdef ENABLE_INOTIFY
	monitor = new MediaMonitor(CARD_ROOT);
#endif

	if (!input.init(this, menu.get())) {
		exit(EXIT_FAILURE);
	}

	powerSaver.setScreenTimeout(confInt["backlightTimeout"]);

#ifdef ENABLE_CPUFREQ
	setClock(confInt["menuClock"]);
#endif
}

GMenu2X::~GMenu2X() {
	fflush(NULL);
	sc.clear();

#ifdef ENABLE_INOTIFY
	delete monitor;
#endif
}

void GMenu2X::initBG() {
	bg.reset();
	bgmain.reset();

	// Load wallpaper.
	bg = OffscreenSurface::loadImage(confStr["wallpaper"]);
	if (!bg) {
		bg = OffscreenSurface::emptySurface(resX, resY);
	}

	drawTopBar(*bg);
	drawBottomBar(*bg);

	bgmain.reset(new OffscreenSurface(*bg));

	{
		auto sd = OffscreenSurface::loadImage(
				sc.getSkinFilePath("imgs/sd.png"));
		if (sd) sd->blit(*bgmain, 3, bottomBarIconY);
	}

	cpuX = 32 + font->write(*bgmain, getDiskFree(getHome().c_str()),
			22, bottomBarTextY, Font::HAlignLeft, Font::VAlignMiddle);
  // sdcard 2 (have to check if exist, and very slow)
  /*font->write(*bgmain, getDiskFree("/media/sdcard"),
			92, bottomBarTextY, Font::HAlignLeft, Font::VAlignMiddle);*/

#ifdef ENABLE_CPUFREQ
	cpuX += 19;
	manualX = cpuX + font->getTextWidth("300MHz") + 5;
#else
	manualX = cpuX;
#endif

	int serviceX = resX-38;
	if (usbnet) {
		if (web) {
			auto webserver = OffscreenSurface::loadImage(
					sc.getSkinFilePath("imgs/webserver.png"));
			if (webserver) webserver->blit(*bgmain, serviceX, bottomBarIconY);
			serviceX -= 19;
		}
		if (samba) {
			auto sambaS = OffscreenSurface::loadImage(
					sc.getSkinFilePath("imgs/samba.png"));
			if (sambaS) sambaS->blit(*bgmain, serviceX, bottomBarIconY);
			serviceX -= 19;
		}
		if (inet) {
			auto inetS = OffscreenSurface::loadImage(
					sc.getSkinFilePath("imgs/inet.png"));
			if (inetS) inetS->blit(*bgmain, serviceX, bottomBarIconY);
			serviceX -= 19;
		}
	}

	bgmain->convertToDisplayFormat();
}

void GMenu2X::initFont() {
	string path = skinConfStr["font"];
	if (!path.empty()) {
		unsigned int size = skinConfInt["fontsize"];
		if (!size)
			size = 12;
		if (path.substr(0,5)=="skin:")
			path = sc.getSkinFilePath(path.substr(5, path.length()));
		font.reset(new Font(path, size));
	} else {
		font = Font::defaultFont();
	}
}

void GMenu2X::initMenu() {
	//Menu structure handler
	menu.reset(new Menu(this, ts));
	for (uint i=0; i<menu->getSections().size(); i++) {
		//Add virtual links in the applications section
		if (menu->getSections()[i]=="applications") {
			menu->addActionLink(i, "Explorer",
					bind(&GMenu2X::explorer, this),
					tr["Launch an application"],
					"skin:icons/explorer.png");
		}

		//Add virtual links in the setting section
		else if (menu->getSections()[i]=="settings") {
			menu->addActionLink(i, "GMenu2X",
					bind(&GMenu2X::showSettings, this),
					tr["Configure GMenu2X's options"],
					"skin:icons/configure.png");
			menu->addActionLink(i, tr["Skin"],
					bind(&GMenu2X::skinMenu, this),
					tr["Configure skin"],
					"skin:icons/skin.png");
			menu->addActionLink(i, tr["Wallpaper"],
					bind(&GMenu2X::changeWallpaper, this),
					tr["Change GMenu2X wallpaper"],
					"skin:icons/wallpaper.png");
			if (fileExists(LOG_FILE)) {
				menu->addActionLink(i, tr["Log Viewer"],
						bind(&GMenu2X::viewLog, this),
						tr["Displays last launched program's output"],
						"skin:icons/ebook.png");
			}
			menu->addActionLink(i, tr["About"],
					bind(&GMenu2X::about, this),
					tr["Info about GMenu2X"],
					"skin:icons/about.png");
			menu->addActionLink(i, "Docs",
					bind(&GMenu2X::docs, this),
					tr["RG350 Docs"],
					"skin:icons/ebook.png");
		}
	}

	menu->skinUpdated();
	menu->orderLinks();

	menu->setSectionIndex(confInt["section"]);
	menu->setLinkIndex(confInt["link"]);

	layers.push_back(menu);
}

void GMenu2X::docs() {
  FileDialog fd(this, ts, tr["Select a doc"], "txt", GMENU2X_SYSTEM_DIR "/docs/"+tr.lang()+"/");
	if (fd.exec()) {
		if (confInt["saveSelection"] && (confInt["section"]!=menu->selSectionIndex() || confInt["link"]!=menu->selLinkIndex()))
			writeConfig();
    string text(readFileAsString(string(fd.getPath()+"/"+fd.getFile()).c_str()));
    TextDialog td(this, "GMenu2X", tr["RG350 Docs"], "icons/ebook.png", text);
    td.exec();
	}
}

void GMenu2X::about() {
	string text(readFileAsString(GMENU2X_SYSTEM_DIR "/about.txt"));
	string build_date("Build date: " __DATE__);
	TextDialog td(this, "GMenu2X", build_date, "icons/about.png", text);
	td.exec();
}

void GMenu2X::viewLog() {
	string text(readFileAsString(LOG_FILE));

	TextDialog td(this, tr["Log Viewer"],
			tr["Displays last launched program's output"],
			"icons/ebook.png", text);
	td.exec();

	MessageBox mb(this, tr["Do you want to delete the log file?"],
			 "icons/ebook.png");
	mb.setButton(InputManager::ACCEPT, tr["Yes"]);
	mb.setButton(InputManager::CANCEL, tr["No"]);
	if (mb.exec() == InputManager::ACCEPT) {
		unlink(LOG_FILE);
		menu->deleteSelectedLink();
	}
}

void GMenu2X::readConfig() {
	string conffile = GMENU2X_SYSTEM_DIR "/gmenu2x.conf";
	readConfig(conffile);
}

void GMenu2X::readConfig(string conffile) {
	ifstream inf(conffile.c_str(), ios_base::in);
	if (inf.is_open()) {
		string line;
		while (getline(inf, line, '\n')) {
			string::size_type pos = line.find("=");
			string name = trim(line.substr(0,pos));
			string value = trim(line.substr(pos+1,line.length()));

			if (value.length()>1 && value.at(0)=='"' && value.at(value.length()-1)=='"')
				confStr[name] = value.substr(1,value.length()-2);
			else
				confInt[name] = atoi(value.c_str());
		}
		inf.close();
	}

	if (!confStr["lang"].empty())
		tr.setLang(confStr["lang"]);

	if (!confStr["wallpaper"].empty() && !fileExists(confStr["wallpaper"]))
		confStr["wallpaper"] = "";

	if (confStr["skin"].empty() || SurfaceCollection::getSkinPath(confStr["skin"]).empty())
		confStr["skin"] = "Default";

	evalIntConf( confInt, "outputLogs", 0, 0,1 );
#ifdef ENABLE_CPUFREQ
	evalIntConf( confInt, "maxClock",
				 cpuFreqSafeMax, cpuFreqMin, cpuFreqMax );
	evalIntConf( confInt, "menuClock",
				 cpuFreqMenuDefault, cpuFreqMin, cpuFreqSafeMax );
#endif
	evalIntConf( confInt, "backlightTimeout", 15, 0,120 );
	evalIntConf( confInt, "buttonRepeatRate", 10, 0, 20 );
	evalIntConf( confInt, "videoBpp", 32, 16, 32 );

	if (confStr["tvoutEncoding"] != "PAL") confStr["tvoutEncoding"] = "NTSC";
	resX = constrain( confInt["resolutionX"], 320,1920 );
	resY = constrain( confInt["resolutionY"], 240,1200 );
}

void GMenu2X::saveSelection() {
	if (confInt["saveSelection"] && (
			confInt["section"] != menu->selSectionIndex()
			|| confInt["link"] != menu->selLinkIndex()
	)) {
		writeConfig();
	}
}

void GMenu2X::writeConfig() {
	string conffile = getHome() + "/gmenu2x.conf";
	ofstream inf(conffile.c_str());
	if (inf.is_open()) {
		ConfStrHash::iterator endS = confStr.end();
		for(ConfStrHash::iterator curr = confStr.begin(); curr != endS; curr++)
			inf << curr->first << "=\"" << curr->second << "\"" << endl;

		ConfIntHash::iterator endI = confInt.end();
		for(ConfIntHash::iterator curr = confInt.begin(); curr != endI; curr++)
			inf << curr->first << "=" << curr->second << endl;

		inf.close();
	}
}

void GMenu2X::writeSkinConfig() {
	string conffile = getHome() + "/skins/";
	if (mkdir(conffile.c_str(), 0770) < 0 && errno != EEXIST) {
		ERROR("Failed to create directory %s to write skin configuration: %s\n", conffile.c_str(), strerror(errno));
		return;
	}
	conffile = conffile + confStr["skin"];
	if (mkdir(conffile.c_str(), 0770) < 0 && errno != EEXIST) {
		ERROR("Failed to create directory %s to write skin configuration: %s\n", conffile.c_str(), strerror(errno));
		return;
	}
	conffile = conffile + "/skin.conf";

	ofstream inf(conffile.c_str());
	if (inf.is_open()) {
		ConfStrHash::iterator endS = skinConfStr.end();
		for(ConfStrHash::iterator curr = skinConfStr.begin(); curr != endS; curr++)
			inf << curr->first << "=\"" << curr->second << "\"" << endl;

		ConfIntHash::iterator endI = skinConfInt.end();
		for(ConfIntHash::iterator curr = skinConfInt.begin(); curr != endI; curr++)
			inf << curr->first << "=" << curr->second << endl;

		int i;
		for (i = 0; i < NUM_COLORS; ++i) {
			inf << colorToString((enum color)i) << "=#"
			    << skinConfColors[i] << endl;
		}

		inf.close();
	}
}

void GMenu2X::readTmp() {
	lastSelectorElement = -1;
	ifstream inf("/tmp/gmenu2x.tmp", ios_base::in);
	if (inf.is_open()) {
		string line;
		string section = "";
		while (getline(inf, line, '\n')) {
			string::size_type pos = line.find("=");
			string name = trim(line.substr(0,pos));
			string value = trim(line.substr(pos+1,line.length()));

			if (name=="section")
				menu->setSectionIndex(atoi(value.c_str()));
			else if (name=="link")
				menu->setLinkIndex(atoi(value.c_str()));
			else if (name=="selectorelem")
				lastSelectorElement = atoi(value.c_str());
			else if (name=="selectordir")
				lastSelectorDir = value;
		}
		inf.close();
	}
}

void GMenu2X::writeTmp(int selelem, const string &selectordir) {
	string conffile = "/tmp/gmenu2x.tmp";
	ofstream inf(conffile.c_str());
	if (inf.is_open()) {
		inf << "section=" << menu->selSectionIndex() << endl;
		inf << "link=" << menu->selLinkIndex() << endl;
		if (selelem>-1)
			inf << "selectorelem=" << selelem << endl;
		if (!selectordir.empty())
			inf << "selectordir=" << selectordir << endl;
		inf.close();
	}
}

void GMenu2X::mainLoop() {
	if (!fileExists(CARD_ROOT))
		CARD_ROOT = "";

	// Recover last session
	readTmp();
	if (lastSelectorElement > -1 && menu->selLinkApp() &&
				(!menu->selLinkApp()->getSelectorDir().empty()
				 || !lastSelectorDir.empty()))
		menu->selLinkApp()->selector(lastSelectorElement, lastSelectorDir);

	while (true) {
		// Remove dismissed layers from the stack.
		for (auto it = layers.begin(); it != layers.end(); ) {
			if ((*it)->getStatus() == Layer::Status::DISMISSED) {
				it = layers.erase(it);
			} else {
				++it;
			}
		}

		// Run animations.
		bool animating = false;
		for (auto layer : layers) {
			animating |= layer->runAnimations();
		}

		// Paint layers.
		for (auto layer : layers) {
			layer->paint(*s);
		}
		s->flip();

		// Exit main loop once we have something to launch.
		if (toLaunch) {
			break;
		}

		// Handle touchscreen events.
		if (ts.available()) {
			ts.poll();
			for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
				if ((*it)->handleTouchscreen(ts)) {
					break;
				}
			}
		}

		// Handle other input events.
		InputManager::Button button;
		bool gotEvent;
		const bool wait = !animating;
		do {
			gotEvent = input.getButton(&button, wait);
		} while (wait && !gotEvent);
		if (gotEvent) {
			for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
				if ((*it)->handleButtonPress(button)) {
					break;
				}
			}
		}
	}
}

void GMenu2X::explorer() {
	FileDialog fd(this, ts, tr["Select an application"], "sh,bin,py,elf,opk,dge,");
	if (fd.exec()) {
		if (confInt["saveSelection"] && (confInt["section"]!=menu->selSectionIndex() || confInt["link"]!=menu->selLinkIndex()))
			writeConfig();

		string command = cmdclean(fd.getPath()+"/"+fd.getFile());
		chdir(fd.getPath().c_str());
#ifdef ENABLE_CPUFREQ
		setClock(cpuFreqAppDefault);
#endif

		toLaunch.reset(new Launcher(
				vector<string> { "/bin/sh", "-c", command }));
	}
}

void GMenu2X::queueLaunch(
	unique_ptr<Launcher>&& launcher, shared_ptr<Layer> launchLayer
) {
	toLaunch = move(launcher);
	layers.push_back(launchLayer);
}

void GMenu2X::showHelpPopup() {
	layers.push_back(make_shared<HelpPopup>(*this));
}

void GMenu2X::showSettings() {
#ifdef ENABLE_CPUFREQ
	int curMenuClock = confInt["menuClock"];
#endif

	FileLister fl_tr;
	fl_tr.setShowDirectories(false);
	fl_tr.browse(GMENU2X_SYSTEM_DIR "/translations");

	vector<string> translations = fl_tr.getFiles();
	translations.insert(translations.begin(), "English");
	string lang = tr.lang();

	vector<string> encodings;
	encodings.push_back("NTSC");
	encodings.push_back("PAL");

	SettingsDialog sd(this, input, ts, tr["Settings"]);
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingMultiString(
			this, ts, tr["Language"],
			tr["Set the language used by GMenu2X"],
			&lang, &translations)));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingBool(
			this, ts, tr["Save last selection"],
			tr["Save the last selected link and section on exit"],
			&confInt["saveSelection"])));
#ifdef ENABLE_CPUFREQ
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingInt(
			this, ts, tr["Clock for GMenu2X"],
			tr["Set the cpu working frequency when running GMenu2X"],
			&confInt["menuClock"], cpuFreqMin, cpuFreqSafeMax, 720)));  // se pone a mano 720 en vez de cpuFreqMultiple, para OBTENER sólo Min o Max (360,1080)
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingInt(
			this, ts, tr["Maximum overclock"],
			tr["Set the maximum overclock for launching links"],
			&confInt["maxClock"], cpuFreqMin, cpuFreqMax, 720)));       // se pone a mano 720 en vez de cpuFreqMultiple, para OBTENER sólo Min o Max  (360,1080)
#endif
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingBool(
			this, ts, tr["Output logs"],
			tr["Logs the output of the links. Use the Log Viewer to read them."],
			&confInt["outputLogs"])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingInt(
			this, ts, tr["Screen Timeout"],
			tr["Set screen's backlight timeout in seconds"],
			&confInt["backlightTimeout"], 0, 120)));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingInt(
			this, ts, tr["Button repeat rate"],
			tr["Set button repetitions per second"],
			&confInt["buttonRepeatRate"], 0, 20)));

	if (sd.exec()) {
#ifdef ENABLE_CPUFREQ
		if (curMenuClock != confInt["menuClock"]) setClock(confInt["menuClock"]);
#endif

		powerSaver.setScreenTimeout(confInt["backlightTimeout"]);

		input.repeatRateChanged();

		if (lang == "English") lang = "";
		if (lang != tr.lang()) {
			tr.setLang(lang);
			confStr["lang"] = lang;
		}

		writeConfig();
	}
}

void GMenu2X::skinMenu() {
	FileLister fl_sk;
	fl_sk.setShowFiles(false);
	fl_sk.setShowUpdir(false);
	fl_sk.browse(GMENU2X_SYSTEM_DIR "/skins");

	string curSkin = confStr["skin"];

	SettingsDialog sd(this, input, ts, tr["Skin"]);
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingMultiString(
			this, ts, tr["Skin"],
			tr["Set the skin used by GMenu2X"],
			&confStr["skin"], &fl_sk.getDirectories())));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Top Bar"],
			tr["Color of the top bar"],
			&skinConfColors[COLOR_TOP_BAR_BG])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Bottom Bar"],
			tr["Color of the bottom bar"],
			&skinConfColors[COLOR_BOTTOM_BAR_BG])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Selection"],
			tr["Color of the selection and other interface details"],
			&skinConfColors[COLOR_SELECTION_BG])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Message Box"],
			tr["Background color of the message box"],
			&skinConfColors[COLOR_MESSAGE_BOX_BG])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Message Box Border"],
			tr["Border color of the message box"],
			&skinConfColors[COLOR_MESSAGE_BOX_BORDER])));
	sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingRGBA(
			this, ts, tr["Message Box Selection"],
			tr["Color of the selection of the message box"],
			&skinConfColors[COLOR_MESSAGE_BOX_SELECTION])));

	if (sd.exec()) {
		if (curSkin != confStr["skin"]) {
			setSkin(confStr["skin"]);
			writeConfig();
		}
		writeSkinConfig();
		initBG();
	}
}

void GMenu2X::setSkin(const string &skin, bool setWallpaper) {
	confStr["skin"] = skin;

	//Clear previous skin settings
	skinConfStr.clear();
	skinConfInt.clear();

	DEBUG("GMenu2X: setting new skin %s.\n", skin.c_str());

	//clear collection and change the skin path
	sc.clear();
	sc.setSkin(skin);

	//reset colors to the default values
	skinConfColors[COLOR_TOP_BAR_BG] = RGBAColor(255, 255, 255, 130);
	skinConfColors[COLOR_BOTTOM_BAR_BG] = RGBAColor(255, 255, 255, 130);
	skinConfColors[COLOR_SELECTION_BG] = RGBAColor(255, 255, 255, 130);
	skinConfColors[COLOR_MESSAGE_BOX_BG] = RGBAColor(255, 255, 255);
	skinConfColors[COLOR_MESSAGE_BOX_BORDER] = RGBAColor(80, 80, 80);
	skinConfColors[COLOR_MESSAGE_BOX_SELECTION] = RGBAColor(160, 160, 160);

	/* Load skin settings from user directory if present,
	 * or from the system directory. */
	readSkinConfig(GMENU2X_SYSTEM_DIR "/skins/" + skin + "/skin.conf");

	if (setWallpaper && !skinConfStr["wallpaper"].empty()) {
		string fp = sc.getSkinFilePath("wallpapers/" + skinConfStr["wallpaper"]);
		if (!fp.empty())
			confStr["wallpaper"] = fp;
		else
			WARNING("Unable to find wallpaper defined on skin %s\n", skin.c_str());
	}

	evalIntConf(skinConfInt, "topBarHeight", 50, 32, 120);
	evalIntConf(skinConfInt, "bottomBarHeight", 20, 20, 120);
	evalIntConf(skinConfInt, "linkHeight", 50, 32, 120);
	evalIntConf(skinConfInt, "linkWidth", 80, 32, 120);

	if (menu != NULL) menu->skinUpdated();

	//Selection png
	useSelectionPng = sc.addSkinRes("imgs/selection.png", false) != NULL;

	//font
	initFont();
}

bool GMenu2X::readSkinConfig(const string& conffile)
{
	ifstream skinconf(conffile.c_str(), ios_base::in);
	if (skinconf.is_open()) {
		string line;
		while (getline(skinconf, line, '\n')) {
			line = trim(line);
			DEBUG("skinconf: '%s'\n", line.c_str());
			string::size_type pos = line.find("=");
			string name = trim(line.substr(0,pos));
			string value = trim(line.substr(pos+1,line.length()));

			if (value.length()>0) {
				if (value.length()>1 && value.at(0)=='"' && value.at(value.length()-1)=='"')
					skinConfStr[name] = value.substr(1,value.length()-2);
				else if (value.at(0) == '#')
					skinConfColors[stringToColor(name)] =
						RGBAColor::fromString(value.substr(1, value.length()));
				else
					skinConfInt[name] = atoi(value.c_str());
			}
		}
		skinconf.close();
		return true;
	} else {
		return false;
	}
}

void GMenu2X::showManual() {
	menu->selLinkApp()->showManual();
}

void GMenu2X::showContextMenu() {
	layers.push_back(make_shared<ContextMenu>(*this, *menu));
}

void GMenu2X::changeWallpaper() {
	WallpaperDialog wp(this, ts);
	if (wp.exec() && confStr["wallpaper"] != wp.wallpaper) {
		confStr["wallpaper"] = wp.wallpaper;
		initBG();
		writeConfig();
	}
}

void GMenu2X::addLink() {
	FileDialog fd(this, ts, tr["Select an application"], "sh,bin,py,elf,opk,dge,");
	if (fd.exec())
		menu->addLink(fd.getPath(), fd.getFile());
}

void GMenu2X::editLink() {
	LinkApp *linkApp = menu->selLinkApp();
	if (!linkApp) return;

	vector<string> pathV;
	split(pathV,linkApp->getFile(),"/");
	string oldSection = "";
	if (pathV.size()>1)
		oldSection = pathV[pathV.size()-2];
	string newSection = oldSection;

	string linkTitle = linkApp->getTitle();
	string linkDescription = linkApp->getDescription();
	string linkIcon = linkApp->getIcon();
	string linkManual = linkApp->getManual();
	string linkSelFilter = linkApp->getSelectorFilter();
	string linkSelDir = linkApp->getSelectorDir();
	bool linkSelBrowser = linkApp->getSelectorBrowser();
	int linkClock = linkApp->clock();

	string diagTitle = tr.translate("Edit $1",linkTitle.c_str(),NULL);
	string diagIcon = linkApp->getIconPath();

	SettingsDialog sd(this, input, ts, diagTitle, diagIcon);
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingString(
				this, ts, tr["Title"],
				tr["Link title"],
				&linkTitle, diagTitle, diagIcon)));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingString(
				this, ts, tr["Description"],
				tr["Link description"],
				&linkDescription, diagTitle, diagIcon)));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingMultiString(
				this, ts, tr["Section"],
				tr["The section this link belongs to"],
				&newSection, &menu->getSections())));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingImage(
				this, ts, tr["Icon"],
				tr.translate("Select an icon for this link", linkTitle.c_str(), NULL),
				&linkIcon, "png")));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingFile(
				this, ts, tr["Manual"],
				tr["Select a manual or README file"],
				&linkManual, "man.png,txt")));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingDir(
				this, ts, tr["Selector Directory"],
				tr["Directory to scan for the selector"],
				&linkSelDir)));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingBool(
				this, ts, tr["Selector Browser"],
				tr["Allow the selector to change directory"],
				&linkSelBrowser)));
#ifdef ENABLE_CPUFREQ
	   sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingInt(
			this, ts, tr["Clock frequency"],
			tr["CPU clock frequency for this link"],
			&linkClock, cpuFreqMin, confInt["maxClock"], 720)));    // se pone a mano 720 en vez de cpuFreqMultiple, para obtener sólo Min o Max  (360,1080)
#endif
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingString(
				this, ts, tr["Selector Filter"],
				tr["Selector filter (Separate values with a comma)"],
				&linkSelFilter, diagTitle, diagIcon)));
		sd.addSetting(unique_ptr<MenuSetting>(new MenuSettingBool(
				this, ts, tr["Display Console"],
				tr["Must be enabled for console-based applications"],
				&linkApp->consoleApp)));

	if (sd.exec()) {
		linkApp->setTitle(linkTitle);
		linkApp->setDescription(linkDescription);
		linkApp->setIcon(linkIcon);
		linkApp->setManual(linkManual);
		linkApp->setSelectorFilter(linkSelFilter);
		linkApp->setSelectorDir(linkSelDir);
		linkApp->setSelectorBrowser(linkSelBrowser);
		linkApp->setClock(linkClock);

		INFO("New Section: '%s'\n", newSection.c_str());

		//if section changed move file and update link->file
		if (oldSection!=newSection) {
			vector<string>::const_iterator newSectionIndex = find(menu->getSections().begin(),menu->getSections().end(),newSection);
			if (newSectionIndex==menu->getSections().end()) return;
			string newFileName = "sections/"+newSection+"/"+linkTitle;
			uint x=2;
			while (fileExists(newFileName)) {
				string id = "";
				stringstream ss; ss << x; ss >> id;
				newFileName = "sections/"+newSection+"/"+linkTitle+id;
				x++;
			}
			rename(linkApp->getFile().c_str(),newFileName.c_str());
			linkApp->renameFile(newFileName);

			INFO("New section index: %i.\n", newSectionIndex - menu->getSections().begin());

			menu->linkChangeSection(menu->selLinkIndex(), menu->selSectionIndex(), newSectionIndex - menu->getSections().begin());
		}
		linkApp->save();
	}
}

void GMenu2X::deleteLink() {
	if (menu->selLinkApp()!=NULL) {
		MessageBox mb(this, tr.translate("Deleting $1",menu->selLink()->getTitle().c_str(),NULL)+"\n"+tr["Are you sure?"], menu->selLink()->getIconPath());
		mb.setButton(InputManager::ACCEPT, tr["Yes"]);
		mb.setButton(InputManager::CANCEL, tr["No"]);
		if (mb.exec() == InputManager::ACCEPT)
			menu->deleteSelectedLink();
	}
}

void GMenu2X::addSection() {
	InputDialog id(this, input, ts, tr["Insert a name for the new section"]);
	if (id.exec()) {
		//only if a section with the same name does not exist
		if (find(menu->getSections().begin(), menu->getSections().end(), id.getInput())
				== menu->getSections().end()) {
			//section directory doesn't exists
			if (menu->addSection(id.getInput()))
				menu->setSectionIndex( menu->getSections().size()-1 ); //switch to the new section
		}
	}
}

void GMenu2X::renameSection() {
	InputDialog id(this, input, ts, tr["Insert a new name for this section"],menu->selSection());
	if (id.exec()) {
		//only if a section with the same name does not exist & !samename
		if (menu->selSection() != id.getInput()
		 && find(menu->getSections().begin(),menu->getSections().end(), id.getInput())
				== menu->getSections().end()) {
			//section directory doesn't exists
			string newsectiondir = getHome() + "/sections/" + id.getInput();
			string sectiondir = getHome() + "/sections/" + menu->selSection();

			if (!rename(sectiondir.c_str(), newsectiondir.c_str())) {
				string oldpng = menu->selSection() + ".png";
				string newpng = id.getInput() + ".png";
				string oldicon = sc.getSkinFilePath(oldpng);
				string newicon = sc.getSkinFilePath(newpng);

				if (!oldicon.empty() && newicon.empty()) {
					newicon = oldicon;
					newicon.replace(newicon.find(oldpng), oldpng.length(), newpng);

					if (!fileExists(newicon)) {
						rename(oldicon.c_str(), newicon.c_str());
						sc.move("skin:"+oldpng, "skin:"+newpng);
					}
				}
				menu->renameSection(menu->selSectionIndex(), id.getInput());
			}
		}
	}
}

void GMenu2X::deleteSection() {
	MessageBox mb(this,tr["You will lose all the links in this section."]+"\n"+tr["Are you sure?"]);
	mb.setButton(InputManager::ACCEPT, tr["Yes"]);
	mb.setButton(InputManager::CANCEL, tr["No"]);
	if (mb.exec() == InputManager::ACCEPT) {

		if (rmtree(getHome() + "/sections/" + menu->selSection()))
			menu->deleteSelectedSection();
	}
}

typedef struct {
	unsigned short batt;
	unsigned short remocon;
} MMSP2ADC;

#ifdef ENABLE_CPUFREQ
void GMenu2X::setClock(unsigned mhz) {
	mhz = constrain(mhz, cpuFreqMin, confInt["maxClock"]);
#if defined(PLATFORM_A320) || defined(PLATFORM_GCW0) || defined(PLATFORM_NANONOTE)
	jz_cpuspeed(mhz);
#endif
}
#endif

string GMenu2X::getDiskFree(const char *path) {
	string df = "";
	struct statvfs b;

	int ret = statvfs(path, &b);
	if (ret == 0) {
		// Make sure that the multiplication happens in 64 bits.
		unsigned long freeMiB =
				((unsigned long long)b.f_bfree * b.f_bsize) / (1024 * 1024);
		unsigned long totalMiB =
				((unsigned long long)b.f_blocks * b.f_frsize) / (1024 * 1024);
		stringstream ss;
		if (totalMiB >= 10000) {
			ss << (freeMiB / 1024) << "." << ((freeMiB % 1024) * 10) / 1024 << "/"
			   << (totalMiB / 1024) << "." << ((totalMiB % 1024) * 10) / 1024 << "GiB";
		} else {
			ss << freeMiB << "/" << totalMiB << "MiB";
		}
		ss >> df;
	} else WARNING("statvfs failed with error '%s'.\n", strerror(errno));
	return df;
}

int GMenu2X::drawButton(Surface& s, const string &btn, const string &text, int x, int y) {
	int w = 0;
	auto icon = sc["skin:imgs/buttons/" + btn + ".png"];
	if (icon) {
		if (y < 0) y = resY + y;
		w = icon->width();
		icon->blit(s, x, y - 7);
		if (!text.empty()) {
			w += 3;
			w += font->write(
					s, text, x + w, y, Font::HAlignLeft, Font::VAlignMiddle);
			w += 6;
		}
	}
	return x + w;
}

int GMenu2X::drawButtonRight(Surface& s, const string &btn, const string &text, int x, int y) {
	int w = 0;
	auto icon = sc["skin:imgs/buttons/" + btn + ".png"];
	if (icon) {
		if (y < 0) y = resY + y;
		w = icon->width();
		icon->blit(s, x - w, y - 7);
		if (!text.empty()) {
			w += 3;
			w += font->write(
					s, text, x - w, y, Font::HAlignRight, Font::VAlignMiddle);
			w += 6;
		}
	}
	return x - w;
}

void GMenu2X::drawScrollBar(uint pageSize, uint totalSize, uint pagePos) {
	if (totalSize <= pageSize) {
		// Everything fits on one screen, no scroll bar needed.
		return;
	}

	unsigned int top, height;
	tie(top, height) = getContentArea();
	top += 1;
	height -= 2;

	s->rectangle(resX - 8, top, 7, height, skinConfColors[COLOR_SELECTION_BG]);
	top += 2;
	height -= 4;

	const uint barSize = max(height * pageSize / totalSize, 4u);
	const uint barPos = (height - barSize) * pagePos / (totalSize - pageSize);

	s->box(resX - 6, top + barPos, 3, barSize,
			skinConfColors[COLOR_SELECTION_BG]);
}

void GMenu2X::drawTopBar(Surface& s) {
	Surface *bar = sc.skinRes("imgs/topbar.png", false);
	if (bar) {
		bar->blit(s, 0, 0);
	} else {
		const int h = skinConfInt["topBarHeight"];
		s.box(0, 0, resX, h, skinConfColors[COLOR_TOP_BAR_BG]);
	}
}

void GMenu2X::drawBottomBar(Surface& s) {
	Surface *bar = sc.skinRes("imgs/bottombar.png", false);
	if (bar) {
		bar->blit(s, 0, resY-bar->height());
	} else {
		const int h = skinConfInt["bottomBarHeight"];
		s.box(0, resY - h, resX, h, skinConfColors[COLOR_BOTTOM_BAR_BG]);
	}
}
