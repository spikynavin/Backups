#!/bin/sh

var=`cat /proc/net/wireless | awk 'NR==3 {print $4}''' | cut -c 2,3`
echo VRA is :$var
if [ $var -le 20 ]
then
echo 5 > /opt/daemon_files/signal_level
elif [ $var -ge 20 ] && [ $var -le 40 ]
then
echo 4 > /opt/daemon_files/signal_level
elif [ $var -ge 40 ] && [ $var -le 60 ]
then
echo 3 > /opt/daemon_files/signal_level
elif [ $var -ge 60 ] && [ $var -le 80 ]
then
echo 2 > /opt/daemon_files/signal_level
else
echo 1 > /opt/daemon_files/signal_level
fi


