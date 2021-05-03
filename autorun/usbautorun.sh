#!/bin/sh
#
# Script to autorun from usb pen-drive
#

echo "Mounting thumb drive ..."

umount /media/thumbdrive
#mkdir /media/thumbdrive
mount /dev/sda1 /media/thumbdrive

if [ -f /media/thumbdrive/autorun.sh  ]
then

        sh /media/thumbdrive/autorun.sh

else
        echo "No autorun script found. Exit now"
fi

exit 0
