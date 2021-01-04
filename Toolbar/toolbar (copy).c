#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#ifndef EV_SYN
#define EV_SYN 0
#endif
#define SHMSZ     27

unsigned int user_timing=0,value=0,backlight_dim=500;  // 500 --> 5 sec
unsigned int check_toolbar=0;
FILE *fp;

main()
{
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) { exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }
    umask(0);
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1)
    {
        value=0;
        check_toolbar++;
        //printf("checking toolbar.. %d\n",check_toolbar);
        if(check_toolbar==2)
        {
            system("sh /opt/toolbar.sh");
            check_toolbar=0;
        }
        sleep(1);
    }
    exit(0);
}

