/*
 * Real Time Clock Driver Test/Example Program
 *
 * Compile with:
 *  gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
 *
 * Copyright (C) 1996, Paul Gortmaker.
 *
 * Released under the GNU General Public License, version 2,
 * included herein by reference.
 *
 */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define RTC_TICK_ON _IO('p', 0x03)  /* Update int. enable on        */
#define RTC_TICK_OFF    _IO('p', 0x04)  /* ... off                      */
#define RTC_TICK_SET    _IO('p', 0x05)  /* Periodic int. enable on      */
#define RTC_TICK_READ   _IO('p', 0x06)  /* ... off                      */
#define RTC_POWEROFF_CUSTOM    _IO('x', 'x')   /* ... off                      */

/*
*setup_alarm():set up the sec,func
*/
int setup_alarm(int fd,int min)
{
    int retval;
    struct rtc_time rtc_tm;
    unsigned long data;

    retval = ioctl(fd, RTC_AIE_OFF, 0);
    if (retval == -1) {
        printf("ioctl RTC_AIE_OFF faild!!!\n");
        return -1;
    }

    /*read current time*/
    retval=ioctl(fd,RTC_RD_TIME,&rtc_tm);
    if (retval <0) {
        printf("ioctl RTC_RD_TIME  faild!!!");
        return -1;
    }

    rtc_tm.tm_min = rtc_tm.tm_min+min;//alarm to 5 sec in the future
    if (rtc_tm.tm_min>60) {
        rtc_tm.tm_min=rtc_tm.tm_min-60;
        rtc_tm.tm_hour=rtc_tm.tm_hour+1;
    }

    /* Set the alarm to 5 sec in the future */
    retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
    if (retval <0) {
        printf("ioctl RTC_ALM_SET  faild!!!\n");
        return -1;
    }

    /* Read the current alarm settings */
    retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
    if (retval <0) {
        printf("ioctl  RTC_ALM_READ faild!!!\n");
        return -1;
    }



    /* Enable alarm interrupts */
    retval = ioctl(fd, RTC_AIE_ON, 0);
    if (retval <0) {
        printf("ioctl  RTC_AIE_ON faild!!!\n");
        return -1;
    }

    /* This blocks until the alarm ring causes an interrupt */
    retval = read(fd, &data, sizeof(unsigned long));
    if (retval >0) {
//        func();
        system("touch /usr/share/status/rtc_interrupt");
    } else {
        printf("!!!alarm faild!!!\n");
        return -1;
    }

    retval = ioctl(fd, RTC_AIE_OFF, 0);
    if (retval == -1) {
        printf("ioctl RTC_AIE_OFF faild!!!\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc==2){
        int fd, retval;

        fd = open ("/dev/rtc", O_RDONLY);
        if (fd<0) {
            printf("open rtc faild!!!\n");
            return -1;
        }

        if(strcmp(argv[1],"-poweroff")==0){
            retval = ioctl(fd, RTC_POWEROFF_CUSTOM,0);
            if (retval <0) {
                printf("ioctl RTC_ALM_SET  faild!!!\n");
                return -1;
            }
        }
        close(fd);
    }else if(argc==3){
        int fd;

        fd = open ("/dev/rtc", O_RDONLY);

        if (fd<0) {
            printf("open rtc faild!!!\n");
            return -1;
        }

        int count=atoi(argv[2]);
        printf("Count: %d\n",count);
        if(strcmp(argv[1],"-set")==0){
            setup_alarm(fd,count);
        }
        close(fd);
    }else{
        printf("Help:\n"
               "    Example: rtc_read -set 10\n"
               "             rtc_read -poweroff\n");
    }

    return 0;

} /* end main */
