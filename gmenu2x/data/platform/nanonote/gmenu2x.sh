#!/bin/sh

source /etc/profile

#Use Nanonote-specific 6x10 font. This font is very clear, but can't display colors
#This results in 53x24 terminal size
#setfont2 /usr/share/setfont2/un-fuzzy-6x10-font.pnm
# This is to display line drawing glyphs correctly (the font itself doesn't contain this information)
#loadunimap /usr/share/setfont2/ben_uni.trans

#Or use standard 6x11 font (supporting colors)
#This results in 53x21 terminal size
#setfont /usr/share/kbd/consolefonts/kernel-6x11-font

#Some people prefer terminus font (supports colors and more glyphs)
#This results in 53x20 terminal size
#setfont /usr/share/kbd/consolefonts/ter-v12n.psf

#We use 6x10 font by default (forked from standard 6x11)
#This results in 53x24 terminal size
setfont /usr/share/kbd/consolefonts/kernel-6x10-font

trap "" hup

clear
exec /usr/bin/gmenu2x.bin
