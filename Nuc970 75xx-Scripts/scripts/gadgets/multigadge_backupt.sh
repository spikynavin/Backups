#!/bin/sh
# Copyright@ClancorTechnovates 2/2015
# Script by Sathya  on 27/2/2015
#
#Script to load g_file_storage and mount backing_file as "CLANCOR" removable disk in HOST PC.
#By default backing_file will be mounted in /media/file_storage directory and can access the files
#/media/file_storage directory only when g_file_storage is unloaded.


start() {
	sh /usr/share/scripts/gadgets/networkgadget.sh stop
	sh /usr/share/scripts/gadgets/serialgadget.sh stop
	sh /usr/share/scripts/gadgets/gadgetfilestorage.sh stop
	sync
        umount /media/file_storage/ 
 	modprobe g_multi file=/home/root/drive/backing_file removable=y
	exec /sbin/getty -L /dev/ttyGS0 115200 vt100 &
}
stop() {
	killall getty
	rmmod g_multi
	mount -t vfat /home/root/drive/backing_file /media/file_storage/
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
