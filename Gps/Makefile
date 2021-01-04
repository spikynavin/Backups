
CC=arm-poky-linux-gnueabi-gcc
CFLAG=-c

gpsd: main func1 func2 func3
	${CC} -o gpsd main.o serial.o gps.o nmea.o -lm

main: main.c
	${CC} ${CFLAG} main.c
func1: serial.c
	${CC} ${CFLAG} serial.c
func2: gps.c
	${CC} ${CFLAG} gps.c
func3: nmea.c
	${CC} ${CFLAG} nmea.c

clean:
	rm -f *.o gpsd

help:
	echo"Sorry no help available"

