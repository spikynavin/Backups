#!/bin/bash


sim_presence_fun()
{

sim_status=`cat /opt/daemon_files/rough_files/sim_presence | awk '{print $2}'`

sim_status=`echo ${sim_status:0:5}`

if [ "$sim_status" = "READY" ]
then

echo "1" > /opt/daemon_files/sim_value

echo "Sim_presence" >> /home/root/GPRS_LOG

else

echo "0" > /opt/daemon_files/sim_value

echo "Sim_not_presence" >> /home/root/GPRS_LOG

fi

}

current_operator_fun()
{

str=`cat /opt/daemon_files/rough_files/current_operator`

if [ "$str" == "+COPS: 2" ]
then

echo "nill" > /opt/daemon_files/signal_status

else

t=$((${#str}-1))
signal_status="${str:$t}"

if [ "$signal_status" == "2" ]
then

echo "3G" > /opt/daemon_files/signal_status

elif [ "$signal_status" == "0" ]
then

echo "2G" > /opt/daemon_files/signal_status

else

current_operator_fun

fi

fi
}

signal_level_fun()
{

signal_status_23=`cat /opt/daemon_files/signal_status`

tower_status=`cat /opt/daemon_files/rough_files/signal_level | awk '{print $2}'`

tower=`echo ${tower_status:0:2}`

if [ "$signal_status_23" == "3G" ]
then

if [ $tower -lt 10 ]
then

echo "11" > /opt/daemon_files/tower_value

elif [ $tower -lt 15 ]
then                
           
echo "12" > /opt/daemon_files/tower_value
                       
elif [ $tower -lt 20 ]
then                
           
echo "13" > /opt/daemon_files/tower_value
                       
elif [ $tower -lt 25 ] 
then                
           
echo "14" > /opt/daemon_files/tower_value
                       
elif [ $tower -lt 30 ] 
then                
           
echo "15" > /opt/daemon_files/tower_value
                       
fi

elif [ "$signal_status_23" == "2G" ]
then

if [ $tower -lt 10 ]                                                          
then                                                                          
                                                                              
echo "6" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 15 ]                                                        
then                                                                          
                                                                              
echo "7" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 20 ]                                                        
then                                                                          
                                                                              
echo "8" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 25 ]                  
then                                    
                                        
echo "9" > /opt/daemon_files/tower_value
                                        
elif [ $tower -lt 30 ]                  
then   

echo "10" > /opt/daemon_files/tower_value                                      
                                        
fi 


elif [ "$signal_status_23" == "nill" ]
then

if [ $tower -lt 10 ]                                                          
then                                                                          
                                                                              
echo "1" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 15 ]                                                        
then                                                                          
                                                                              
echo "2" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 20 ]                                                        
then                                                                          
                                                                              
echo "3" > /opt/daemon_files/tower_value                                      
                                                                              
elif [ $tower -lt 25 ]                  
then                                    
                                        
echo "4" > /opt/daemon_files/tower_value
                                        
elif [ $tower -lt 30 ]                  
then   

echo "5" > /opt/daemon_files/tower_value                                      
                                        
fi 


fi

}

creg_fun()
{

creg=`cat /opt/daemon_files/rough_files/creg | awk '{print $2}'`

creg_1=`echo ${creg:0:1}`
creg_2=`echo ${creg:2:1}`

#echo "$creg_1 , $creg_2" >> /home/root/GPRS_LOG

if [ "$creg_2" == "0" ]
then

echo "at+creg=1" > /dev/ttyACM5

fi

}

case $1 in

	"sim_presence") sh /opt/daemon_files//GPRS_status.sh $1 | grep +CPIN > /opt/daemon_files/rough_files/$1
			cat /opt/daemon_files/rough_files/$1 >> /home/root/GPRS_LOG
			sim_presence_fun
	;;
	
	"current_operator") sh /opt/daemon_files//GPRS_status.sh $1 | grep +COPS > /opt/daemon_files/rough_files/$1
			cat /opt/daemon_files/rough_files/$1 >> /home/root/GPRS_LOG
			current_operator_fun
	;;

	"signal_level") 
			sim=`cat /opt/daemon_files/sim_value`
			if [ "$sim" == "1" ]
			then

			sh /opt/daemon_files//GPRS_status.sh $1 | grep +CSQ > /opt/daemon_files/rough_files/$1
			cat /opt/daemon_files/rough_files/$1 >> /home/root/GPRS_LOG
			signal_level_fun

			else
			
			echo "0" > /opt/daemon_files/tower_value
			
			fi
	;;

	"creg") 
			sim=`cat /opt/daemon_files/sim_value`
			if [ "$sim" == "1" ]
			then

			sh /opt/daemon_files//GPRS_status.sh $1 | grep +CREG > /opt/daemon_files/rough_files/$1
			cat /opt/daemon_files/rough_files/$1 >> /home/root/GPRS_LOG
#			creg_fun

			else
			
			echo "0" > /opt/daemon_files/tower_value
			
			fi
	;;
esac


