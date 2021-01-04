#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "gps.h"
#include "nmea.h"
#include "serial.h"

#define FILE_PATH "/home/root/file.txt"

int main(void) {

    /*****************************Daemon******************************/
    pid_t pid=0;
    pid_t sid=0;
    pid = fork();
    if (pid < 0) { exit(1); }
    if (pid > 0) { exit(0); }
    umask(0);
    sid = setsid();
    if (sid < 0) { exit(1); }
    chdir("/");
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    /*****************************Daemon******************************/
    sleep(1);
    // Open
    gps_init();
    usleep(100000);
    FILE *fp;
    loc_t data;

    while (1) {

        gps_location(&data);

        //printf("%lf %lf %f %d %ld\n", data.latitude, data.longitude, data.altitude, data.speed, data.course);

        fp = fopen(FILE_PATH,"w");
        fprintf(fp,"latitude        -%lf\n",data.latitude);
        fprintf(fp,"longitude       -%lf\n",data.longitude);
        fprintf(fp,"time utc        -Null\n");
        if(data.altitude>=50 && data.altitude<=8000)
        {
            fprintf(fp,"altitude (m)    -%.2f\n",data.altitude);
        }
        else
        {
            fprintf(fp,"altitude (m)    -0\n");
        }
        if(data.speed>=5 && data.speed<=150)
        {
            fprintf(fp,"speed (m/s)     -%d\n",data.speed);
        }
        else
        {
            fprintf(fp,"speed (m/s)     -0\n");
        }
        fflush(fp);
        fclose(fp);

        sleep(1);

    }

    return EXIT_SUCCESS;
}

