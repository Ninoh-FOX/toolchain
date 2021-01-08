#!/bin/sh

[ -z "$1" ] || [ "x$1" = "xstart" ] || exit 0

# Play a dummy WAV.
# This fixes the audio hardware not working properly after reset.
/usr/bin/aplay /usr/share/bootsound/dummy.wav >/dev/null 2>&1
