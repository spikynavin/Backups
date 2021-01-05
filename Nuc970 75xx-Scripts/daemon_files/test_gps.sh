#!/bin/bash


cat /dev/ttyS8 > /opt/os/test &

sleep 1

killall cat </dev/null &

size=`ls -lh /opt/os/test | awk '{print $5}'`

if [ "$size" == "0" ]
then

echo 0 > /opt/os/gps_sat

else

echo 1 > /opt/os/gps_sat

fi
