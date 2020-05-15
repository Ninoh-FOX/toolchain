#!/bin/sh

if [ -f /media/data/.finish ]; then
   exit 1
 else
   echo -n 1 >/sys/devices/virtual/vtconsole/vtcon1/bind
   clear
   echo "Finishing installation."
   echo "This will prevent data corruption in case the console"
   echo "did not shutdown properly on the first system boot."
   sleep 4
   echo "done"
   sleep 2
   echo "The system reboot in 10s..."
   sleep 10
   echo -n 0 >/sys/devices/virtual/vtconsole/vtcon1/bind
   touch /media/data/.finish
   reboot
 fi