# GMenuNX

[GMenuNX](https://github.com/pingflood/GMenuNX/) is a fork of [GMenu2X](http://mtorromeo.github.com/gmenu2x) developed to Retrogame RS-97, released under the GNU GPL license v2.

View releases [changelog](ChangeLog.md).


## Installation

Unpack the latest [Release](https://github.com/pingflood/GMenuNX/releases/) over your current GMenu2X directory, or download the latest binary and replace it in your device.

(Re)Boot your device and enjoy [GMenuNX](https://github.com/pingflood/GMenuNX/).


## Controls

* A: Accept / Launch selected link / Confirm action;
* B: Back / Cancel action / Goes up one directory in file browser;
* Y: Bring up the manual/readme;
* L, R: Switch between sections / PageUp/PageDown on lists;
* START: GMenu2X settings;
* SELECT: Bring up the contextual menu;
* SELECT + L: Volume control;
* SELECT + R: Take a screenshot;
* BRIGHTNESS: Change screen brightness;
* POWER: Toggle Suspend mode;
* POWER (hold): Bring up Poweroff dialog.
* TV-Out: (Dis)Connect the TV-out plug to toggle TV-out signal.

In settings:

* A, LEFT, RIGHT: Accept, select or toggle setting;
* B: Back or cancel edits and return;
* START: Save edits and return;
* SELECT: Clear or set default value.


## [How to have previews in Selector Browser](http://boards.dingoonity.org/ingenic-jz4760-devices/gmenunext-let's-make-gmenu-great-again!/msg177392/#msg177392)

* Select the link you want to edit and press "menu";
* Edit the link;
* Configure the link to match your directory structure. Important fields:
	* Selector Directory: The directory of your roms
	* Selector Browser: Enable selector before launching the app
	* Selector Filter: Filter extensions to be shown in the selector. Separe multiple extensions with commas.
	* Selector Screenshots: The directory of the screenshots/preview of your roms. It can be different than your roms directory.
* The name of the file of rom and preview have to be exactly the same. Suported image types are .png or .jpg;


## How to create battery logs

To get data of your battery charge and discharge cycle:
* Enter the Battery Logger;
* Do a full charge;
* After charged, remove the cable;
* Stay in this screen and wait until it discharges totally;

Repeat how many times you wish and can.

New data will be printed on screen every minute and will be saved in file **battery.csv** located in **gmenu2x** folder.

The fields logged are:
* Time: Time in milliseconds since GMenuNX started;
* BatteryStatus: Computed battery status, from 0 (discharged) to 4 (charged) and 5 (charging);
* BatteryLevel: Raw battery level as given by system.


## Thanks

I want to thanks [@msx](https://boards.dingoonity.org/profile/msx/), [@m1024x](https://boards.dingoonity.org/profile/m1024/) and [@jutley](https://boards.dingoonity.org/profile/jutley/) for all the support they are giving, testing and reporting bugs.


## Contacts

GMenu2X Copyright (c) 2006-2010 [Massimiliano Torromeo](mailto:massimiliano.torromeo@gmail.com); GMenuNX 2018 by [@pingflood](https://boards.dingoonity.org/profile/pingflood/);

Visit the [Dingoonity thread](https://boards.dingoonity.org/ingenic-jz4760-devices/gmenunext-let's-make-gmenu-great-again!/) and the [Discord channel](https://discord.gg/hvR5vK6)!

[GMenu2X](http://mtorromeo.github.com/gmenu2x) homepage for more info.
