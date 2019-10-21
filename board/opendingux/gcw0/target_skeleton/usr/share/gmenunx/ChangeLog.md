## Changelog

**2018.09.16** (*Codename: No more tears (AKA: My work is done here).*)
* Renamed fork to GMenuNX;
* New gridded menu layout:
	* Allow to customize number of rows and columns in settings;
	* If column number is set to 1, will use list layout;
* Do not hardlink icon and manual when adding a new link;
* Properly detect TV out on boot;
* Improved font rendering engine;
* Default CPU clock to 600MHz and max CPU to 624 MHz;
* (Try to) Fix SD card mounting mess introduced in Useless CFW;
* Add some workarounds trying make USB mount work better;
	* Force reboot on UDC unmount (sorry, but needed to prevent more corruption due to crappy filesystem management in CFWs);
* Updated Simplified Chinese translation; Thanks to Luckyhgq;
* Prevent Suspend mode while using BatteryLogger;
* Fix Selector browsing directory up when selector browser is off;
* Assert '..' directory is the first in list;

**2018.07.18** (*Codename: Low Carb.*)
* Updated Selector screen previews;
* Remove SDL_gfx dependecies by importing gmenu2x-gcw0 surface methods;
* Set default cpuMin to 342MHz;
* Unify file browser routines;
* File Explorer:
	* Support image preview;
	* View image files in ImageDialog;
	* View text files in TextDialog;
* Settings:
	* Add color switch for bool items;
	* New menu type allow submenus;
	* Add Factory Reset sub menu;
	* Reorder menu items;
	* Merge wallpaper and appearance settings into Skin menu;
* Auto detect Headphones;
* Auto detect external SD Card;
* Batch of internal changes, bug fixes and optimizations.

**2018.07.02** (*Codename: Blast Processing.*)
* Fix memory address register:
	* Fix SetCPU not setting CPU clock;
	* TV-Out working among SetCPU;
* Do not use inputCommonActions in MessageBox:
	* Prevents power button bounce;
	* Prevents entering in suspend mode while a messagebox is on screen;
* Updated compiler flags (optimize size and performance);
* UDC autodetect mount point when booting on external SD;
* Simplify Link and LinkAction classes;
* Use memregs to get UDC status (faster than system function);
* Releases will bundle only default GMenu2X assets:
	* Cleaned up default assets;
	* Allow to use icons/wallpapers/backdrops pack.

**2018.06.23** (*Codename: UUDDLRLRBA.*)
* Settings allow to change Section Bar position;
* Revamped TV-out function:
	* Plug/unplug the TV-out connector to enable/disable TV-out;
* Change CPU clock values safe range to 312~642MHz;
	* Allow to set max/min/default CPU clock values by editing gmenu2x.conf;
* New class PowerManager:
	* Use of SDL_AddTimer to manage Suspend and Poweroff;
	* Get rid of maths with SDL_GetTicks;
* Optimize button press and repeat detection:
	* Fine tune button timings;
* Use /dev/mem to monitor device status (faster than system function);
* Rework Link Scanner dialog;
* Rework of external SD card mount/umount;
* Fix Text Viewer dialog top position;
* Improve automatic loading of Manual files;
* Removed all extra skins:
	* Releases will bundle only Default skin;
* Re-add unicode font supporting Chinese, Korean, Japanese and Russian languages;
* Batch of internal changes and bug fixes.

**2018.06.03** (*Codename: McFly.*)
* New Date & Time settings:
	* Allow to set system's date and time (works until reboot);
* Fix bug when returning from app/emu to GMenuNX;
* Fix bug with Select combo key triggering context menu;
* Add new Volume slider control (SELECT + L):
	* Now you can control volume via software and hardware;
* Auto load available Manual files on start;
* Use the same app backdrop in Manual reader;
* Save Last Selection properly remember latest Section and Link;
* Updated some translatable text strings;
* Support loading and converting ancient skin files with separated RGBA components;
* Search for icon/backdrop/manual named as app linkname and foldername;
* Live skin:
	* Auto update layout and spacing on change;
	* Auto update font color and size on change;
* Allow set CPU clock values between 210 and 642 MHz (safe range);
* New lightweight font;
* New button input routine - try to improve button presses and repeats;
* Adjusts in titlebar text position;
* Add Korean translation, thanks to [@haven-jeon](https://github.com/haven-jeon);
* Batch of internal changes and bug fixes.

**2018.05.22** (*Codename: Zeraora.*)
* Improved loop timers in main thread and dialogs:
	* Enable SDL_AddTimer for RS97;
* Combo button is now SELECT:
	* SELECT + R: Screenshot;
	* SELECT + L: Toggle sound;
* Improved powerManager function:
	* New option to turn device off when inactive;
	* Add power manager to all dialogs;
* Optimize dialogs:
	* Don't redraw title, list and and bottom bar on every loop;
	* Pass title, description and icon as argument to wallpaper dialog;
* Improve manual dialog:
	* Integrate text manual with text dialog;
	* Create ImageViewer dialog;
* Add section icon to 'Rename Section' and 'Add Section' dialogs;
* Fix settings dialog redraw transparency;
* Realtime skin preview:
	* Update title icon on change skin;
    * Update button images on change skin;
* Fix INC/DEC buttons (update your input.conf!);
* Adjust buttons actions in menus and dialogs;
* globalVolume is fixed in RS97, as there's no way to adjust volume via software;
* Merged Toggle TV in TV system selection in settings:
	* TV OFF: Will use RS97's display;
	* NTSC / PAL: Will output image in TV-out jack and turn display off;
	* Will apply the setting on save and will reset to OFF on reboot;
* Reuse of gmenu2x->input prevents reinstantiate InputManager;
* Use global gmenu2x->inputCommonActions and avoid code duplication;
* Improvements in backlight slider;
* Batch of internal changes and bug fixes.

**2018.05.12** (*Codename: Any color you like.*)
* Realtime preview of Skin changes;
* Allow customize dimensions in Skin settings;
* Tweaks in MessageBox text position;
* Updated [@m1024x](https://github.com/m1024x/retrogame-skin-pack) retrogame skin pack.
* Fixed bug with fontAlt color in dialogs;
* Fixed bug when adding new link;
* Fix browse dialog not returning directory on exit;
* Batch of internal changes:
	* Updated some translatable strings;
	* Initialize fontAlt with #fd01fc00 and use regular color if not changed by skin;
	* Convert Link Scanner into an external dialog;
	* Convert Battery Logger into an external dialog;
	* Do not compile GP2X specific code to RS97;
	* Allow font color as argument to FontHelper write function.

**2018.05.06** (*Codename: Blind shot.*)
* Moved Battery Logger to Applications;
* Shows Battery Logger only if enter GMenuNX while charging;
* Use QWERTY keyboard layout in editor screens;
* Implement an alternative font color suport in skins;
* Add option to Skins backdrops;
* Fix crash when changing wallpaper in default skin;
* Fix backdrop not working in Selector screen;
* Updated [@m1024x](https://github.com/m1024x/retrogame-skin-pack) skin pack;
* Batch of internal changes.

**2018.05.02** (*Codename: Your device, your backdrops.*)
* Image backdrops in main menu and selector;
* Page up/down in settings dialog;
* Page up/down and rollover in context menu;
* Batch of internal changes:
	* Cleanup unused strings;
	* Improved path handler;
	* Unified use of SDL_Rect listRect for lists.

**2018.04.30** (*Codename: LWE.*)
* Removed Disk Free space from About Screen due to system lag.
* Removed /dist from repository and start using "Releases" 

**2018.04.28** (*Codename: Edison still wrong.*)
* Option to choose battery discharge profile in Settings;
* Uptime and battery status in About Screen;
* Improvements in Battery Logger;
* Calc text height in fonthelper;
* Messagebox use text height to create dialog
* Second adjust in battery indicator levels;
* Adjusts in About Screen.

**2018.04.26** (*Codename: How about you?*)
* Add some system information in About screen;
* About screen using about.txt

**2018.04.25** (*Codename: Edison was wrong.*)
* Save screenshot in a ./screenshots folder;
* First adjust in battery indicator based in data provided by Battery Logging. Thanks to @Valenhir, @m1024x and @Jutley for logging and sharing battery data. ;)

**2018.04.24** (*Codename: Snapshots of a Memory.*)
* Fixed incorrect brightness when returning from suspend mode;
* Dynamic fill the icon tray;
* Added option to auto hide a MessageBox;
* Use MessageBox to draw some messages;
* Save screenshot with combo [X] + [BUTTON_R];
* Mute/unmute with combo [X] + [TRIGGER_L];
* Fix in DekUI messagebox color;
* Screenshot function is global
* Brightness control is global

**2018.04.23** (*Codename: Don't look back.*)
* Fixed the Log Viewer;
* Added default values for brightness and global volume;
* New poweroff menu;
* New brightness slider control;
* New battery logger screen;
* New linear fit for battery level measurements (waiting for logs to improve);
* Rearranged how all input buttons are handled in main menu;
* Added option to customize the inactive timeout to suspend. Values can be set set between 10 to 300 seconds;
* Improved how to trigger power off and suspend via power button:
     * If hold the power button for ~1s, it will suspend the device;
     * If hold the power button for ~2.5s, power off menu will pop up;
* When waking up from suspended state, a press in power menu should wake up the device with at least 10% of brightness level;
* Rearranged in "core" of the main menu where it redraws the screen. Prevent redraw screen while in "suspend" mode.
* Absolute path resolving in filelister; Prevents (?) buffer overflow after browsing a lot of files and allow resolving relative paths.

**2018.04.20** (*Codename: Here comes a new challenger.*)
* New main menu layout, with a section bar on the left and items in list, inspired in [concept designed by @m1024x](http://boards.dingoonity.org/ingenic-jz4760-devices/gmenunext-let's-make-gmenu-great-again!/msg177170/#msg177170);
* Retouched every screen to make all system cohesive;
* Alternative selector browser, allowing you to load previews of your games if selector previews is set;
* Navigate page up/down with directional left/right;
* New variables for skin designers;
* New set of "Next" skins by [@m1024x](https://github.com/m1024x/retrogame-skin-pack).


-----------
New in 0.12
-----------
Built-in ginge support (credits for ginge go to notaz)
Modified default skins to use bitmapped ttf fonts that look better at smaller sizes
Fixed fade-in/out of some menu transitions
Fixed support for alpha channel of the font colors
Fixed saving of color settings
Fixed various layout imperfections

-----------
New in 0.11
-----------
Wiz and Caanoo support implemented (including battery readings and overclocking)
Completely rewritten font code: fonts are now standard truetype fonts with a 1px outline.
Skins can specify a custom font, change its size and the color of the outline and of the font itself.
Completely rewritten input system: the screen is redrawn only when it needs to be updated, idling when there is no input by the user.
Additional system informations displayed in the about dialog
Cpu clock increments by steps of ten
File selectors from the "edit link" menu now start browsing from the link directory
Reduced size of the binary
Lots of bug fixes

-----------
New in 0.10
-----------
Raised default clock to 136Mhz for F200 models (minimum frequency for touchscreen)
Fixed bug in link navigation
Support for additional utf8 characters
Restructured the virtual keyboard dialog with touchscreen support
New translations: Basque, Catalan, Turkish
Added support for the touchscreen
Added support for f200 battery
Added internal explorer that can launch .gpu .gpe and .sh files
Added graphic indicators for switching sections
Added on screen help (keep pressed A, only on the main screen at the moment)
Cpu clock raised when loading manuals
Added support for bmp and jpg/jpeg files for manuals
Added preview of images when selecting icons
Added option to show the root folder in the selection dialogs
Fixed bug that prevented the use of the custom selected wallpaper on restart
Lots of fixes

----------
New in 0.9
----------
Added gamma setting (global and per-link)
Added option to tweak ram timings
Added skins support
Added link to activate tv-out
Added support for multiple wallpapers
Color of message dialogs changeable by skins
Automatic icon association for links with icons found in the skin "icons" directory
Adding a link to a file without gpu or gpe extension automatically enables the wrapper feature
Added support for translations (Italian, Danish, Dutch, Spanish, German, French, Finnish, Portuguese, Swedish, Slovak, Norwegian and Russian included)
Use of a new font code which supports (some) utf8 characters
Added graphics for gp2x buttons
Moved screenshot in the selector under the text
[selFullPath] is automatically added at the end of the params when using the selector and no [marker] is specified
Easier access to the last row of links
Moved manual indicator to the bottom bar
Save screenshot with A+L+R (only on the main screen)
SD corruption when using USB on certain setups should be fixed
Fixed half tv-out screen
Fixed flickering in various locations
Fixed bugs in the manuals navigation system
Other small changes

------------
New in 0.8.1
------------
Fixed crash on section creation.
Fixed moving links between sections.
Fixed creating links in the first section.
Fixed alpha support for font.png

----------
New in 0.8
----------
Added support for game manuals (.man.png, .man.txt or readmes)
Implemented link's output logs with a log viewer (Disabled by default)
Implemented a simple text reader used in various situations
Implemented volume control (per-link and globally)
Added some confirmation dialogs
Changed cancel button from A to X
Added link to mount nand on usb with a dialog notification if usb is used for networking
Implemented virtual links (gmenu2x settings, usb, log viewer, about)
Added messageboxes
Added links for firmware 2.1.x
Directories browseable from selector
Added a blending effect to selector screenshots.
Updated the README with basic instructions (website is still recommended)
Fixed some minor bugs.

----------
New in 0.7
----------
GMenu2X always returns to last section/game/selector used (until shutdown)
New params magic value: [selFullPath] which equals [selPath][selFile][selExt]
Params magic values ([selFile] & Co.) are now automatically escaped, so quotes are not needed (and may cause problems)
Added support for an optional alias file for the selector that display the full name of roms.
Fixed bug with links with special characters.
Removed some unneeded savings.
Number of rows and columns on the links page configurable.
Various little fixes and tweaks

----------
New in 0.6
----------
Implemented link scanner
Added options to links for using a selector with them
Initial support for usbnet, ftp, telnet, samba and web server
Battery led indicates disk writes (don't turn the gp2x off while the led is on)
L = PageUp, R = PageDown in lists
Raised maximum hardcoded clock to 325Mhz and default gmenu2x clock to 100Mhz
Improved virtual keyboard
Sorting functions are now case unsensitive (filebrowser, dirdialog, selector)
Wrapper script removed and fixed a related bugs (wrapper function is still available)
Link's section changeable
All link's features are configurable from the interface
Battery indicator refined
File selector starts from /mnt instead of /mnt/sd
Fixed support for fonts of different size

----------
New in 0.5
----------
Added context-menu option to change link icon
Added options to configure top bar, bottom bar and selection color separately.
Readded support for imgs/selection.png (used only if present)
GMenu2X is now working at 50Mhz (configurable)
Added ability to set the maximum overclock to use with links (hardcoded maximum remains 300Mhz)
Moved under/overclock functions in gmenu2x (no need for cpu_speed binary)
"Save last selection" feature disableable

----------
New in 0.4
----------
Fixed flickering in context menu
Fixed bug that prevented correct launch of some applications
Interface color configurable (START brings up an options menu) (removed img/selection.png)
Saves and restores last selected section and link
Added global config file
Sections with no icon are displayed with a default one (icons/section.png)
Added virtual keyboard for editing title and description of links
Links created through file browser are truncated if too long

----------
New in 0.3
----------
Links creatable in-menu using a file browser
Links deletable in-menu
Icon associated with gpu/gpe is used when no icon is specified
Dithered background
Added battery status indicator
Added a menu with options binded to the select button
Clock setting is saved when changed
Links area scrollable to allow more than 24 links per section
Fix for links with DOS line-endings
Increased space between icons

------------
New in 0.2.1
------------
No more crash in presence of a section with no valid links
Keys and values in links are now trimmed. This means that you can spaces before and after keys and values (IE: " exec = /path/to/link.gpe  " is valid)
Split applications section in applications and settings and added settings/exit link that launches the original frontend.
L and R shoulder buttons swapped

----------
New in 0.2
----------
Interface completely redesigned, now there are 24 links visible at the same time.
Display of free space on sd
In-Menu Under/Overclocker using the included utility by god_at_hell, Hermes/PS2Reality, Robster, Vimacs (edit script/cpuspeed.sh to tweak other parameteres besides clock speed)
Any number of sections supported
Customisation made simplier (just replace bg.png with any wallpaper of size 320x240)
