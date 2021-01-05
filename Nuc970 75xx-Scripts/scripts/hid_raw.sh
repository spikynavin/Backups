#!/bin/sh
case $1 in
	start) var=`cat /usr/share/status/backlight_read_time`
		if [ $var -ne "0" ]
		then
			if [ -f "/usr/share/status/backlight_backup" ]
			then
			   cp /usr/share/status/backlight_read_time /usr/share/status/backlight_backup
			   sh /usr/share/scripts/backlight 4
			   echo 0 > /usr/share/status/backlight_read_time
			fi
		fi
	;;
	stop) 
		var1=`cat /usr/share/status/backlight_backup`   
		echo $var1 > /usr/share/status/backlight_read_time
		rm /usr/share/status/backlight_backup
	;;
esac
