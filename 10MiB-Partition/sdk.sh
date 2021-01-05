#!/bin/bash
case $1 in
"start")
	echo `cat /usr/share/status/KEYPAD_buzzer` > /proc/keypad/KEYPAD_buzzer
	if [ -f "/usr/share/status/backlight_backup" ]
	then
		/bin/cp /usr/share/status/backlight_backup /usr/share/status/backlight_read_time
		/bin/rm /usr/share/status/backlight_backup
	fi
	sh /opt/daemon_files/GPIO.sh
	insmod /opt/daemon_files/printer.ko
	insmod /opt/daemon_files/mt7601Usta.ko
	insmod /opt/daemon_files/pn5xx_i2c.ko
	/usr/sbin/crond
	/opt/daemon_files/keyd -d
	/opt/daemon_files/standbyd -d
	/opt/daemon_files/taskd -d
	/opt/daemon_files/netd -d
	/opt/daemon_files/gpiod
	/opt/daemon_files/rmgmtd -d
	echo "attaching user partiton"
	if [ -r /dev/ubi1 ]; then
		ubidetach /dev/ubi_ctrl -m 3
		sync
	fi
	ubiattach /dev/ubi_ctrl -m 3
	mount -t ubifs ubi1:user /media/nand
;;
"stop")
	sh /usr/share/scripts/gadgets/serialgadget.sh stop
	rmmod printer
	rmmod mt7601Usta
	rmmod pn5xx_i2c
	killall crond
	killall keyd
	killall standbyd
	killall netd
	killall gpiod
	killall rmgmtd
	umount /media/nand
	ubidetach /dev/ubi_ctrl -m 3
;;
esac

#/opt/sdk/bin/ClanCor_HHC -qws -display VNC:LinuxFb 2>/opt/os/log/Clancor.log &
