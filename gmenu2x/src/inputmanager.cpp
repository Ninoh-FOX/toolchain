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

#include "debug.h"
#include "inputmanager.h"
#include "gmenu2x.h"
#include "utilities.h"
#include "powersaver.h"
#include "menu.h"

#include <iostream>
#include <fstream>

using namespace std;

bool InputManager::init(GMenu2X *gmenu2x, Menu *menu) {
	this->gmenu2x = gmenu2x;
	this->menu = menu;

	repeatRateChanged();

	for (int i = 0; i < BUTTON_TYPE_SIZE; i++) {
		buttonMap[i].js_mapped = false;
		buttonMap[i].kb_mapped = false;
	}

	/* If a user-specified input.conf file exists, we load it;
	 * otherwise, we load the default one. */
	string input_file = GMENU2X_SYSTEM_DIR "/input.conf";
	DEBUG("Loading user-specific input.conf file: %s.\n", input_file.c_str());
	if (!readConfFile(input_file)) {
			ERROR("InputManager: failed to open config file\n");
			return false;
		}

	return true;
}

InputManager::InputManager(PowerSaver& powerSaver)
	: powerSaver(powerSaver)
{
#ifndef SDL_JOYSTICK_DISABLED
	int i;

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
		ERROR("Unable to init joystick subsystem\n");
		return;
	}

	for (i = 0; i < SDL_NumJoysticks(); i++) {
		struct Joystick joystick = {
			SDL_JoystickOpen(i), false, false, false, false,
			SDL_HAT_CENTERED, nullptr, this,
		};
		joysticks.push_back(joystick);
	}

	DEBUG("Opening %i joysticks\n", i);
#endif
}

InputManager::~InputManager()
{
#ifndef SDL_JOYSTICK_DISABLED
	for (auto it : joysticks)
		SDL_JoystickClose(it.joystick);
#endif
}

bool InputManager::readConfFile(const string &conffile) {
	ifstream inf(conffile.c_str(), ios_base::in);
	if (inf.is_open()) {
		string line;
		while (getline(inf, line, '\n')) {
			string::size_type pos = line.find("=");
			string name = trim(line.substr(0,pos));
			line = trim(line.substr(pos+1,line.length()));

			Button button;
			if (name == "up")            button = UP;
			else if (name == "down")     button = DOWN;
			else if (name == "left")     button = LEFT;
			else if (name == "right")    button = RIGHT;
			else if (name == "accept")   button = ACCEPT;
			else if (name == "cancel")   button = CANCEL;
			else if (name == "altleft")  button = ALTLEFT;
			else if (name == "altright") button = ALTRIGHT;
			else if (name == "menu")     button = MENU;
			else if (name == "settings") button = SETTINGS;
			else {
				WARNING("InputManager: Ignoring unknown button name \"%s\"\n",
						name.c_str());
				continue;
			}

			pos = line.find(",");
			string sourceStr = trim(line.substr(0,pos));
			line = trim(line.substr(pos+1, line.length()));

			if (sourceStr == "keyboard") {
				buttonMap[button].kb_mapped = true;
				buttonMap[button].kb_code = atoi(line.c_str());
	#ifndef SDL_JOYSTICK_DISABLED
			} else if (sourceStr == "joystick") {
				buttonMap[button].js_mapped = true;
				buttonMap[button].js_code = atoi(line.c_str());
	#endif
			} else {
				WARNING("InputManager: Ignoring unknown button source \"%s\"\n",
						sourceStr.c_str());
				continue;
			}
		}

		inf.close();
		return true;
	} else {
		return false;
	}
}

InputManager::Button InputManager::waitForPressedButton() {
	Button button;
	while (!getButton(&button, true));
	return button;
}

static int repeatRateMs(int repeatRate)
{
	return repeatRate == 0 ? 0 : 1000 / repeatRate;
}

void InputManager::repeatRateChanged() {
	int ms = repeatRateMs(gmenu2x->confInt["buttonRepeatRate"]);
	if (ms == 0) {
		SDL_EnableKeyRepeat(0, 0);
	} else {
		SDL_EnableKeyRepeat(INPUT_KEY_REPEAT_DELAY, ms);
	}
}

bool InputManager::pollButton(Button *button) {
	return getButton(button, false);
}

bool InputManager::getButton(Button *button, bool wait) {
	//TODO: when an event is processed, program a new event
	//in some time, and when it occurs, do a key repeat

#ifndef SDL_JOYSTICK_DISABLED
	if (joysticks.size() > 0)
		SDL_JoystickUpdate();
#endif

	SDL_Event event;
	if (wait)
		SDL_WaitEvent(&event);
	else if (!SDL_PollEvent(&event))
		return false;

  // si está en modo salvapantallas y pulsamos power, activamos la pantalla
  if(!powerSaver.getState() && event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_HOME) {
    powerSaver.resetScreenTimer();
    return false;
  }
  else
    if(!powerSaver.getState())    // si estamos en modo salvapantallas, volvemos sin procesar nada
      return false;

	bool is_kb = false, is_js = false;
	switch(event.type) {
		case SDL_KEYDOWN:
			is_kb = true;
		  // si pulsamos power, activamos el salvapantallas
      if(event.key.keysym.sym==SDLK_HOME)
        if(powerSaver.getState()) {
          powerSaver.enablePowerSaver();
          return false;
        }
			break;
#ifndef SDL_JOYSTICK_DISABLED
		case SDL_JOYHATMOTION: {
				Joystick *joystick = &joysticks[event.jaxis.which];
				joystick->hatState = event.jhat.value;

				switch (event.jhat.value) {
					case SDL_HAT_CENTERED:
						stopTimer(joystick);
						return false;
					case SDL_HAT_UP:
						*button = UP;
						break;
					case SDL_HAT_DOWN:
						*button = DOWN;
						break;
					case SDL_HAT_LEFT:
						*button = LEFT;
						break;
					case SDL_HAT_RIGHT:
						*button = RIGHT;
						break;
				}
				startTimer(joystick);
			}
#endif
		case SDL_USEREVENT:
			switch ((enum EventCode) event.user.code) {
#ifdef HAVE_LIBOPK
				case REMOVE_LINKS:
					menu->removePackageLink((const char *) event.user.data1);
					break;
				case OPEN_PACKAGE:
					menu->openPackage((const char *) event.user.data1);
					break;
				case OPEN_PACKAGES_FROM_DIR:
					menu->openPackagesFromDir(
								((string) (const char *) event.user.data1
								 + "/apps").c_str());
					break;
#endif /* HAVE_LIBOPK */
        case REPAINT_MENU:
				default:
					break;
			}

			if (event.user.data1)
				free(event.user.data1);
			*button = REPAINT;
			return true;

		default:
			return false;
	}

	int i = 0;
	if (is_kb) {
		for (i = 0; i < BUTTON_TYPE_SIZE; i++) {
			if (buttonMap[i].kb_mapped
					&& (unsigned int)event.key.keysym.sym == buttonMap[i].kb_code) {
				*button = static_cast<Button>(i);
				break;
			}
		}
#ifndef SDL_JOYSTICK_DISABLED
	} else if (is_js && event.type == SDL_JOYBUTTONDOWN) {
		for (i = 0; i < BUTTON_TYPE_SIZE; i++) {
			if (buttonMap[i].js_mapped
					&& (unsigned int)event.jbutton.button == buttonMap[i].js_code) {
				*button = static_cast<Button>(i);
				break;
			}
		}
#endif
	}

	if (i == BUTTON_TYPE_SIZE)
		return false;

	if (wait && powerSaver.getState()) {
		powerSaver.resetScreenTimer();
	}

	return true;
}

Uint32 keyRepeatCallback(Uint32 timeout, void *d)
{
	struct Joystick *joystick = (struct Joystick *) d;
	return joystick->inputManager->joystickRepeatCallback(timeout, joystick);
}

void InputManager::startTimer(Joystick *joystick)
{
	if (joystick->timer)
		return;

	joystick->timer = SDL_AddTimer(INPUT_KEY_REPEAT_DELAY,
				keyRepeatCallback, joystick);
}

Uint32 InputManager::joystickRepeatCallback(Uint32 timeout __attribute__((unused)), struct Joystick *joystick)
{
	Uint8 hatState;

	if (joystick->axisState[1][AXIS_STATE_NEGATIVE])
		hatState = SDL_HAT_UP;
	else if (joystick->axisState[1][AXIS_STATE_POSITIVE])
		hatState = SDL_HAT_DOWN;
	else if (joystick->axisState[0][AXIS_STATE_NEGATIVE])
		hatState = SDL_HAT_LEFT;
	else if (joystick->axisState[0][AXIS_STATE_POSITIVE])
		hatState = SDL_HAT_RIGHT;
	else
		hatState = joystick->hatState;

	SDL_JoyHatEvent e = {
		.type = SDL_JOYHATMOTION,
		.which = (Uint8) SDL_JoystickIndex(joystick->joystick),
		.hat = 0,
		.value = hatState,
	};
	SDL_PushEvent((SDL_Event *) &e);

	return repeatRateMs(gmenu2x->confInt["buttonRepeatRate"]);
}

void InputManager::stopTimer(Joystick *joystick)
{
	if (joystick->timer) {
		SDL_RemoveTimer(joystick->timer);
		joystick->timer = nullptr;
	}
}
