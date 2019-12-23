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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL.h>
#include <string>
#include <vector>

#define INPUT_KEY_REPEAT_DELAY 250

class GMenu2X;
class Menu;
class PowerSaver;
class InputManager;

// añadido EMPTY para evitar eventos fantasma con code=0
enum EventCode {
  EMPTY,
	REMOVE_LINKS,
	OPEN_PACKAGE,
	OPEN_PACKAGES_FROM_DIR,
	REPAINT_MENU,
};

#ifndef SDL_JOYSTICK_DISABLED
#define AXIS_STATE_POSITIVE 0
#define AXIS_STATE_NEGATIVE 1
struct Joystick {
	SDL_Joystick *joystick;
	bool axisState[2][2];
	Uint8 hatState;
	SDL_TimerID timer;
	InputManager *inputManager;
};
#endif

class InputManager {
public:
// añadido el valor EMPTY para que, en caso de producirse una pulsación fantasma, no se procese
	enum Button {
	  EMPTY,
		UP, DOWN, LEFT, RIGHT,
		ACCEPT, CANCEL,
		ALTLEFT, ALTRIGHT,
		MENU, SETTINGS,
		REPAINT,
	};
	#define BUTTON_TYPE_SIZE 11     // botones a procesar, todos excepto REPAINT

	InputManager(PowerSaver& powerSaver);
	~InputManager();

	bool init(GMenu2X *gmenu2x, Menu *menu);
	Button waitForPressedButton();
	void repeatRateChanged();
	Uint32 joystickRepeatCallback(Uint32 timeout, struct Joystick *joystick);
	bool pollButton(Button *button);
	bool getButton(Button *button, bool wait);

private:
	bool readConfFile(const std::string &conffile);

	struct ButtonMapEntry {
		bool kb_mapped, js_mapped;
		unsigned int kb_code, js_code;
	};

	GMenu2X *gmenu2x;
	Menu *menu;
	PowerSaver& powerSaver;

	ButtonMapEntry buttonMap[BUTTON_TYPE_SIZE];
#ifndef SDL_JOYSTICK_DISABLED
	std::vector<Joystick> joysticks;

	void startTimer(Joystick *joystick);
	void stopTimer(Joystick *joystick);
#endif
};

#endif
