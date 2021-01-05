#!/bin/bash
/usr/bin/ntpdate -b -s -u pool.ntp.org
if [ $? -eq 0 ]
then
	echo 1 > /usr/share/status/timesync
	/sbin/hwclock -w
fi
