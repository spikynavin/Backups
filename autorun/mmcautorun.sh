#!/bin/sh
#
# Script to autorun from usb sdcard for update and patch applications
#

#echo "Mounting sdcard ..."
if [ -b  /dev/mmcblk0p1 ]
then
#mkdir /media/sdcard
#umount /media/sdcard
mount /dev/mmcblk0p1 /media/sdcard

if [ -f /media/sdcard/autorun.sh ]
then
        sh /media/sdcard/autorun.sh
else
echo "No autorun script found. Exit now"
fi
fi
exit 0
