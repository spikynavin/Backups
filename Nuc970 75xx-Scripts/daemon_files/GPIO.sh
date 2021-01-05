#Display Reset
	echo 37 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio37/direction
#Battery Status 1
	echo 290 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio290/direction
#Battery Status 2
	echo 292 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio292/direction
#3.8v Regulator
	echo 42 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio42/direction
#5v Regulator
	echo 43 > /sys/class/gpio/export
        echo out > /sys/class/gpio/gpio43/direction
#GPRS Enable
	echo 288 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio288/direction
#GPRS Reset
	echo 289 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio289/direction
#Sim Presence
	echo 169 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio169/direction
#Audio Enable
	echo 167 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio167/direction
#GPS Enable
	echo 174 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio174/direction
#Buzzer Enable
	echo 195 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio195/direction
#Future Enable
	echo 163 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio163/direction
#RFID Enable
#	echo 35 > /sys/class/gpio/export
#	echo out > /sys/class/gpio/gpio35/direction
#Finger Print Enable
	echo 166 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio166/direction
#Barcode Enable
        echo 196 > /sys/class/gpio/export           
        echo out > /sys/class/gpio/gpio196/direction
#Barcode Trigger
        echo 197 > /sys/class/gpio/export           
        echo out > /sys/class/gpio/gpio197/direction
#Paper Sensing                                    
        echo 162 > /sys/class/gpio/export           
        echo in > /sys/class/gpio/gpio162/direction
#GPRS Wakeup                                     
        echo 194 > /sys/class/gpio/export           
        echo out > /sys/class/gpio/gpio194/direction 
#GPS 3D Fix                                       
        echo 173 > /sys/class/gpio/export          
        echo in > /sys/class/gpio/gpio173/direction
#PWM Export
	echo 0 > /sys/class/pwm/pwmchip0/export
	echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
#	echo 100000 > /sys/class/pwm/pwmchip0/pwm0/period
#AC Detect
	echo 110 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio110/direction
#RFID IO Enable
#	echo 109 > /sys/class/gpio/export
#	echo out > /sys/class/gpio/gpio109/direction
#CAMERA REG Enable 1
	echo 168 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio168/direction
#CAMERA REG Enable 2
	echo 111 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio111/direction
#Hub Enable
	echo 143 > /sys/class/gpio/export                   
	echo out > /sys/class/gpio/gpio143/direction
#Wifi Enable
	echo 164 > /sys/class/gpio/export 
	echo in > /sys/class/gpio/gpio164/direction

#Hub ON
	echo 1 > /sys/class/gpio/gpio43/value
	echo 1 > /sys/class/gpio/gpio143/value

if [ -d /media/sdcard ]                             
then                                                
        rmdir /media/sdcard                         
fi                                                  
if [ -d /media/thumbdrive ]                         
then                                                
        rmdir /media/thumbdrive                     
fi
