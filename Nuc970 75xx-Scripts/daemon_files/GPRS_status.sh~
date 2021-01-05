#!/bin/bash


case $1 in

	"sim_presence") echo "AT+CREG=1" > /dev/mux0
			cat /dev/mux0 /opt/daemon_files/hello &
			sleep 1
			echo -e "\n"
			chat -V -s '' 'at+cpin?' '' > /dev/mux0 < /dev/ttymux0
			sleep 2
			killall cat
			exit 0
	;;

	"current_operator") cat /dev/mux0 /opt/daemon_files/hello &
			sleep 1
			echo -e "\n"
			chat -V -s '' 'at+cops?' '' > /dev/mux0 < /dev/mux0
			sleep 2
			killall cat
			exit 0
	;;
	
	"signal_level") cat /dev/mux0 /opt/daemon_files/hello &
			sleep 1
			echo -e "\n"
			chat -V -s '' 'at+csq?' '' > /dev/mux0 < /dev/mux0
			sleep 2
			killall cat
			exit 0
	;;

	"creg") cat /dev/mux0 /opt/daemon_files/hello &
			sleep 1
			echo -e "\n"
			chat -V -s '' 'at+creg?' '' > /dev/mux0 < /dev/mux0
			sleep 2
			killall cat
			exit 0
	;;

esac
