#!/bin/bash
# Copyright@ClancorTechnovates 2/2015
# Script by Sathya  on 27/2/2015
#
#Script to load g_file_storage and mount backing_file as "CLANCOR" removable disk in HOST PC.
#By default backing_file will be mounted in /media/file_storage directory and can access the files
#/media/file_storage directory only when g_file_storage is unloaded.

start() {
	echo "MachID start initiated"
	sh /usr/share/scripts/gadgets/usbhost.sh stop
	sh /usr/share/scripts/gadgets/gadgetfilestorage.sh stop
	sh /usr/share/scripts/gadgets/serialgadget.sh stop
	killall machid
	echo 1 > /sys/class/gpio/gpio167/value
	sleep 0.5
	modprobe g_serial
        /opt/daemon_files/machid -d
	echo "MachID start completed"
}
stop() {
	echo "MachID stop initiated"
	killall machid
	sleep 0.5
	modprobe -r g_serial
	modprobe -r usb_f_acm
	modprobe -r u_serial
	echo 0 > /sys/class/gpio/gpio167/value
	echo "MachID stop completed"
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
