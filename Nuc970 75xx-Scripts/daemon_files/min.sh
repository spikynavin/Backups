var=`cat /opt/daemon_files/winid`
xdotool windowunmap $var
cdata=$1
case "$cdata" in
	1)
		xdotool windowmap $var
	;;
	0)
		xdotool windowunmap $var
	;;
esac
