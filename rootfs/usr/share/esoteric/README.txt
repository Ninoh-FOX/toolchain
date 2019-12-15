# Esoteric

[Esoteric](https://github.com/podulator/esoteric/) is a fork of [GMenu2X](http://mtorromeo.github.com/gmenu2x) developed for the ANBERNICRG-350 - (hence 350teric), released under the GNU GPL license v2.

View releases [changelog](ChangeLog.md).

## Installation

Copy the latest [Release](https://github.com/podulator/esoteric/releases/) opk to your RG-350, and launch it.

If you decide you want to install it, there is an option to do that in the Settings menu.
You can uninstall it as the default launcher the same way.

## Controls

- D-Pad L, R: PageUp/PageDown on lists;
- D-Pad U, D: Up, Down and wrap on lists
- A: Accept / Launch selected link / Confirm action;
- B: Back / Cancel action / Goes up one directory in file browser;
- Y: Bring up the manual/readme;
- L1, R1: Switch between sections / PageUp/PageDown on lists;
- L2, R2: Jump to next letter in a list
- START: System settings;
- SELECT: Bring up the contextual menu;

In settings:

- A, LEFT, RIGHT: Accept, select or toggle setting;
- B: Back or cancel edits and return;
- START: Save edits and return;
- SELECT: Clear or set default value.

## How to have previews in Selector Browser

- Create a folder called `.previews` in a rom folder
- Fill it full of shiny
- The name of the file (without filetype suffix) of rom and preview have to be exactly the same.
- Suported image types are .png or .jpg;
- To change how the previews are shown, see the [Skinning]("#Skinning) section

## How to filter files in Selector Browser

- Create a file called `.filters` in a rom browser
- Add a csv of values to allow, eg. `.gba,.snes,.smc,.cue`
- This is a temporary work around until we get the opk's updated

## How to create battery logs

To get data of your battery charge and discharge cycle:

- Enter the Battery Logger;
- Do a full charge;
-* After charged, remove the cable;
- Stay in this screen and wait until it discharges totally;

Repeat how many times you wish and can.

New data will be printed on screen every minute and will be saved in file **battery.csv** located in your **.gmenunx** folder.

The fields logged are:

- Time: Time in milliseconds since Esoteric started;
- BatteryStatus: Computed battery status, from 0 (discharged) to 4 (charged) and 5 (charging);
- BatteryLevel: Raw battery level as given by system.

## Thanks 

All at [Retro Game Handhelds](https://discord.gg/29DrhQf)

## Contacts

GMenu2X Copyright (c) 2006-2010 [Massimiliano Torromeo](mailto:massimiliano.torromeo@gmail.com); 
GMenuNX 2018 by [@pingflood](https://boards.dingoonity.org/profile/pingflood/);
Esoteric 2019 and beyond by [@podulator](podulator#5243);

Visit the [Discord channel](https://discord.gg/29DrhQf)!

[Esoteric](http://podulator.github.com/esoteric) homepage for more info.
