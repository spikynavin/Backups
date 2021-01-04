#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "mf_api.h"

#define baudrate 115200
void main(void)
{
    int fd;
    fd = open("/dev/ttyACM0",O_RDWR | O_NOCTTY);
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
    SerialPortSettings.c_cc[VMIN] = 10;
    SerialPortSettings.c_cc[VTIME] = 0;


    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");

    tcflush(fd, TCIFLUSH);
    char read_buffer[32];
    int  bytes_read = 0;

    printf("\nWaiting For Request..!\n");
    memset(read_buffer,0,sizeof(read_buffer));
    while(1)
    {
        usleep(500);
        bytes_read = read(fd,&read_buffer,1024);
        if(bytes_read > 0)
        {
            printf("Data Received: %s\n",read_buffer);
        }
        memset(read_buffer,'\0',sizeof(read_buffer));
    }
    close(fd);
}
