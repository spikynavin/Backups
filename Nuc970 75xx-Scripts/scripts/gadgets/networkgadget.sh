#!/bin/sh
# Copyright@ClancorTechnovates 2/2015
# Script by Sathya  on 27/2/2015
#
#Script to load g_file_storage and mount backing_file as "CLANCOR" removable disk in HOST PC.
#By default backing_file will be mounted in /media/file_storage directory and can access the files
#/media/file_storage directory only when g_file_storage is unloaded.


start() {
	sh /usr/share/scripts/gadgets/multigadget.sh stop
	sh /usr/share/scripts/gadgets/gadgetfilestorage.sh stop
	sh /usr/share/scripts/gadgets/serialgadget.sh stop
	echo 1 > /sys/class/gpio/gpio167/value
	sleep 0.5
	modprobe g_ether
	ifup usb0
}
stop() {
	ifdown usb0
	rmmod g_ether
	echo 0 > /sys/class/gpio/gpio167/value
}
restart() {
        stop
        start
}

case "$1" in
  start)
       start 
       ;;
  stop)
       stop 
       ;;
  restart|reload)
       restart
       ;;
  *)
       echo $"Usage: $0 {start|stop|restart}"
       exit 1
esac

exit $?
