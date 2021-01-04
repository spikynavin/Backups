#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    FILE *file;
    char datetime[8],hours[2],minutes[2],seconds[2],finaltime[8];
    int ihours,iminutes;
    strcpy(datetime,argv[1]);
    memcpy(hours,datetime,2);
    memcpy(minutes,&datetime[3],2);
    memcpy(seconds,&datetime[6],2);

    ihours=atoi(hours)+atoi(argv[2]);
    iminutes=atoi(minutes)+atoi(argv[3]);

    if(ihours>=24)
    {
        ihours=ihours-24;
    }
    if(iminutes>=60)
    {
        ihours=ihours+1;
        iminutes=iminutes-60;
    }
    printf("Hours: %d\n",ihours);
    printf("Minutes: %d\n",iminutes);

    if(ihours<10)
    {
        strcat(finaltime,"0");
        sprintf(hours, "%d", ihours);
        strcat(finaltime,hours);
        strcat(finaltime,":");
    }
    else
    {
        sprintf(hours, "%d", ihours);
        strcat(finaltime,hours);
        strcat(finaltime,":");
    }

    if(iminutes<10)
    {
        strcat(finaltime,"0");
        sprintf(minutes, "%d", iminutes);
        strcat(finaltime,minutes);
        strcat(finaltime,":");
    }
    else
    {
        sprintf(minutes, "%d", iminutes);
        strcat(finaltime,minutes);
        strcat(finaltime,":");
    }

    strcat(finaltime,seconds);
    printf("Time: %s\n",finaltime);

    file=fopen("/home/root/addTimeZone","w+");
    fprintf(file,"%s",finaltime);
    fclose(file);

    return 0;
}

