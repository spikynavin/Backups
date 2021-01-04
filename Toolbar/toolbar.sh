#!/bin/sh

#var=`ps | grep "toolbar"| awk {'print $5'} | cut -c 14,15,16,17`
#var2=`ps | grep "toolbar"| awk {'print $5'}`
#echo $var2
#var=`echo $var2| cut -c 14,15,16,17`
#echo $var
var=`ps | grep "/usr/bin/toolbar"| awk {'print $5'}`
var1=`echo $var | awk {'print $1'}`
var2=`echo $var | awk {'print $2'}`

echo $var1
echo $var2

echo $var > /tmp/tmpval
#echo $var2 > /tmp/tmptmp1
#echo testttt

if [ $var1 == "/usr/bin/toolbar" ]
then
echo $var
#export DISPLAY=:0.0
elif [ $var2 == "/usr/bin/toolbar" ]
then
echo $var2
else
#echo not running
echo $var > /tmp/tmptmp
echo $var2 > /tmp/tmptmp1
export DISPLAY=:0.0
killall toolbar
exec /usr/bin/toolbar &
sleep 5
fi


toolval=`cat /tmp/toolvalue`
if [ $toolval == "0" ]
then
sleep 2
toolval2=`cat /tmp/toolvalue`
if [ $toolval2 == "0" ]
then
echo toolbar hanged!!!
export DISPLAY=:0.0
killall toolbar
exec /usr/bin/toolbar &
sleep 5
fi
else
echo 0 > /tmp/toolvalue
fi 
