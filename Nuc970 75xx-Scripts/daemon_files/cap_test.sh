ls /dev/video* > /dev/null
if [ $echo $? == 0 ]
then
echo 1 > /opt/daemon_files/cap_sat
else
echo 0 > /opt/daemon_files/cap_sat
fi

