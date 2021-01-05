status=`lsusb | grep Sagem | awk '{print $7}'`

if [ "$status" == "Sagem" ]
then
	echo 1 > /opt/daemon_files/fp_sat
else
	echo 0 > /opt/daemon_files/fp_sat
fi

