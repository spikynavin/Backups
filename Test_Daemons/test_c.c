#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char data[20];
    fd = open("/home/elinux1/test",O_RDWR | O_NOCTTY );
    read(fd,data,sizeof(data));
    printf("Data: %s",data);
    close(fd);
    return 0;
}
