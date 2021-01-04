#include <stdio.h>
#include <fcntl.h>   
#include <termios.h> 
#include <unistd.h> 
#include <errno.h>

#define baudrate 115200

void main(void)
{
    int fd;
    fd = open("/dev/ttyACM0",O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1)	
        printf("\n  Error! in Opening ttyACM0  ");
    else
        printf("\n  ttyACM0 Opened Successfully ");

    struct termios SerialPortSettings;
    tcgetattr(fd, &SerialPortSettings);
    cfsetispeed(&SerialPortSettings,baudrate);
    cfsetospeed(&SerialPortSettings,baudrate);

    SerialPortSettings.c_cflag &= ~PARENB;
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |=  CS8;
    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    SerialPortSettings.c_oflag &= ~OPOST;

    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");

    /*------------------------------- Write data to serial port -----------------------------*/

    char write_buffer[] = "A";	
    int  bytes_written  = 0;  	

    bytes_written = write(fd,write_buffer,sizeof(write_buffer));
    
    printf("\n  %s written to ttyUSB0",write_buffer);
    printf("\n  %d Bytes written to ttyUSB0", bytes_written);
    printf("\n +----------------------------------+\n\n");

    close(fd);

}

