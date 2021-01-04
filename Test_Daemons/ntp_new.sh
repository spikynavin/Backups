#!/bin/bash
export DISPLAY=:0.0
#ntpdate -b -s -u pool.ntp.org

#if [ "$?" == "0" ]
#then

ref_date=`date +"%Y-%m-%d"`
ref_time=`date +"%T"`

/usr/sbin/date_bin N +5:30 $ref_date $ref_time

cur_date=`cat /home/root/addTimeZone`

rm /home/root/addTimeZone

date -s "$cur_date"

#sleep 5

hwclock -w -f /dev/rtc1
sleep 1
killall ntpd
#fi
