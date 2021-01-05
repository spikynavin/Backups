#!/bin/sh
case $1 in
"start") 
	echo 1 > /opt/daemon_files/standby_status
	cp /opt/daemon_files/nw_status /opt/daemon_files/nw_status_backup
	sh /usr/share/scripts/backlight 1
	echo 3 > /proc/sys/vm/drop_caches
	;;
"stop")
	echo 0 > /opt/daemon_files/standby_status
	cp /opt/daemon_files/nw_status_backup /opt/daemon_files/nw_status
	rm /opt/daemon_files/nw_status_backup
	sh /usr/share/scripts/backlight 4
	;;
esac
