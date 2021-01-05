case "$1" in                                                                    
start)                                                                          
        var=`ps | grep '[k]eyd\|[t]askd\|[g]piod\|[g]psd\|[n]etd' | awk '{print $1}'`
	kill -STOP $var
	;;                                                                      
                                                                                
stop)                                                                           
        var=`ps | grep '[k]eyd\|[t]askd\|[g]piod\|[g]psd\|[n]etd' | awk '{print $1}'`
	kill -CONT $var
	;;
*)
echo "Usage: /opt/daemon_files/pkill {start|stop}"                       
        exit 1                                                                  
        ;;                                                                      
esac
