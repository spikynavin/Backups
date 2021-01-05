#!/bin/sh
#
# Script to autorun from usb sdcard
#
#
# Copyright@ClancorTechnovates

echo "Mounting sdcard ..."
if [ -b  /dev/mmcblk0p1 ]
then
mkdir /media/sdcard
umount /media/sdcard
mount /dev/mmcblk0p1 /media/sdcard

#if [ -f /media/sdcard/patch.clan ] || [ -f /media/sdcard/application.clan ]

#then
#	sh /usr/share/scripts/backlight 4
#	cd /usr/bin/
#	export DISPLAY=:0.0;
#	./clan_installer

if [ -f /media/sdcard/autorun.sh ]
then

	sh /media/sdcard/autorun.sh
	
else

echo "No autorun script found. Exit now"

fi
fi
exit 0

