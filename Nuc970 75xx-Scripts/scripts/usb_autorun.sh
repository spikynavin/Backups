#!/bin/sh
#
# Script to autorun from usb pen-drive
#
# Copyright@ClancorTechnovates

echo "Mounting thumb drive ..."

umount /media/thumbdrive
mkdir /media/thumbdrive
mount /dev/sda1 /media/thumbdrive

#if [[ -f /media/thumbdrive/application.clan ] || [  -f /media/thumbdrive/patch.clan ]]
#then
#	sh /usr/share/scripts/backlight 4
#	cd /usr/bin/
#	export DISPLAY=:0.0
#	./clan_installer
if [ -f /media/thumbdrive/autorun.sh  ]
then

	sh /media/thumbdrive/autorun.sh

else
	echo "No autorun script found. Exit now"
fi

exit 0

