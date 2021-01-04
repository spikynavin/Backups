#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <dirent.h>

#define SHMSZ     27

char buff[20];
char bat_value[5];
char charger_status1[2];
char charger_status2[2];
char charger_status3[2];
char task_bar_status[15];
char date_status[20];
char tower[10];
char nw_status[10];
char remote_task_bar;

int charger_notify=0;

unsigned int bat_NoOfBytes;
unsigned int bat_Temp=0;
unsigned int bat_level=0;
unsigned char bat_Dummy=0;
int low_bat_count=0, leve0_count=0, leve1_count=0,leve2_count=0, leve3_count=0, leve4_count=0, leve5_count=0;

int a;

int i=0,bat_count=0,bat_status_check=0,present_level,level_changes=5,level_up,ntp=0;
char level;

pid_t proc_find(const char* name)
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}

int main(int argc, char *argv[])
{
    if( argc == 2 ) {
        if(strcmp(argv[1],"-d")==0)
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
        }
    }
    else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
    }
    else {
        printf("Debug Mode.\n");
    }

    char c, gpssat[3];
    FILE *fp_bat;
    FILE *fp_status1;
    FILE *fp_tower;
    FILE *fp_nw;
    FILE *fp_batv,*fbatv,*fp_gps;
    char batvolt[4];

    /*Shared Memory Creation*/
    int shmid;
    key_t key;
    char *shm, *s;
    key = 2345;
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    s = shm;
    /*Shared Memory Creation*/

    int tmp_bat=9999;

    task_bar_status[0]='^';
    task_bar_status[1]=0x30;//Tower Level
    task_bar_status[2]=0x30;//Tower Level
    task_bar_status[3]='~';
    task_bar_status[4]=0x30;//GPS notify
    task_bar_status[5]='~';
    task_bar_status[6]=0x30;//Network Status
    task_bar_status[7]='~';
    task_bar_status[8]='+';//Charger Status
    task_bar_status[9]=0x30;//Battery Level
    task_bar_status[10]='!';

    while(1)
    {
        if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat");
            exit(1);
        }
        s = shm;

        task_bar_status[0]='^';
        task_bar_status[1]=0x30;
        task_bar_status[2]=0x30;
        task_bar_status[3]='~'; //GPS notify
        task_bar_status[4]=0x30;
        task_bar_status[5]='~';
        task_bar_status[6]=0x30;
        task_bar_status[7]='~';
        task_bar_status[8]='+';
        //        task_bar_status[9]=0x30;
        task_bar_status[10]='!';

        fp_tower = fopen("/opt/daemon_files/tower_value", "r");
        if(fp_tower)
        {
            fread(tower, 1, 2, fp_tower);
            fclose(fp_tower);
        }

        //        printf("Char-1: %c\n",tower[0]);
        //        printf("Char-2: %c\n",tower[1]);

        a=atoi(tower);
        //        printf("Tower Value: %d\n",a);

        if(a>=0 && a<=9)
        {
            task_bar_status[1]=0x30;
            task_bar_status[2]=tower[0];
        }
        else if(a>=10 && a<=20)
        {
            task_bar_status[1]=tower[0];
            task_bar_status[2]=tower[1];
        }

        fp_gps = fopen("/opt/sdk/resources/gps_file","r");
        if(fp_gps)
        {
            fread(gpssat,3,1,fp_gps);
            fclose(fp_gps);
        }
        if(gpssat[0]=='G')
        {
            task_bar_status[4]=0x31;
            if(system("ps | grep [g]psd > /dev/null")!=0)
            {
                system("/opt/daemon_files/gpsd");
                //                printf("Launching Gps Daemon\n");
                sleep(1);
            }
        }
        else
        {
            task_bar_status[4]=0x30;
        }

        fp_nw = fopen("/opt/daemon_files/ping_status", "r");
        if(fp_nw)
        {
            fread(nw_status, 1, 1, fp_nw);
            fclose(fp_nw);
        }

        if(nw_status[0]== 'E' ||nw_status[0]== 'e' ||nw_status[0]== 'W' || nw_status[0]== 'w' || nw_status[0]== 'G' || nw_status[0]== 'g')
        {
            task_bar_status[6]=nw_status[0];
        }
        else
        {
            task_bar_status[6]=0x30;
        }

        fp_batv = fopen("/sys/class/power_supply/NUC970Bat/voltage_now","r");
        if(fp_batv)
        {
            fscanf(fp_batv,"%s",batvolt);
            fclose(fp_batv);
        }

        int vol;
        vol=atoi(batvolt);

        fbatv = fopen("/opt/daemon_files/bat_level","w");
        if(fbatv)
        {
            fwrite(batvolt,1,sizeof(batvolt),fbatv);
            fclose(fbatv);
        }

        fp_status1 = fopen("/sys/class/gpio/gpio110/value", "r");
        if(fp_status1)
        {
            fread(charger_status1, 1, 1, fp_status1);
            fclose(fp_status1);
        }
        if(charger_status1[0] == '0')
        {
            if(vol>=4885)
            {
                task_bar_status[8]='+';
                task_bar_status[9]='9';
            }
            else
            {
                task_bar_status[8]='+';
                task_bar_status[9]='1';
            }
            tmp_bat=9999;
            charger_notify=1;
        }
        else if(charger_status1[0] == '1')
        {
            if(vol>=4885)
            {
                task_bar_status[8]='-';
                task_bar_status[9]='F';
            }
            else
            {
                task_bar_status[8]='-';
            }
            charger_notify=0;
        }

        if(charger_notify==0){
            fp_bat = fopen("/sys/class/power_supply/NUC970Bat/present", "r");
            if(fp_bat)
            {
                fscanf(fp_bat,"%d",&bat_Temp);
                fclose(fp_bat);
            }
            printf("Battery Tmp:%d\n",bat_Temp);
            if(bat_Temp<=tmp_bat){
                if(bat_Temp<100 && bat_Temp>=90){
                    task_bar_status[9]=0x35;
                    low_bat_count=0;
                    leve1_count=0;
                    leve2_count=0;
                    leve3_count=0;
                    leve4_count=0;
                }
                else if(bat_Temp<=89 && bat_Temp>=88) {
                    leve4_count++;
                    printf("Level 4 Battery Count: %d\n",leve4_count);
                    if(leve4_count>10){
                        task_bar_status[9]=0x34;
                        low_bat_count=0;
                        leve1_count=0;
                        leve2_count=0;
                        leve3_count=0;
                        leve4_count=11;
                    } else {
                        task_bar_status[9]=0x35;
                        low_bat_count=0;
                        leve1_count=0;
                        leve2_count=0;
                        leve3_count=0;
                    }
                }
                else if(bat_Temp<=87 && bat_Temp>=86) {
                    printf("Level 3 Battery Count: %d\n",leve3_count);
                    leve3_count++;
                    if(leve3_count>10){
                        task_bar_status[9]=0x33;
                        low_bat_count=0;
                        leve1_count=0;
                        leve2_count=0;
                        leve3_count=11;
                    } else {
                        task_bar_status[9]=0x34;
                        low_bat_count=0;
                        leve1_count=0;
                        leve2_count=0;
                    }
                }
                else if(bat_Temp<=85 && bat_Temp>=84) {
                    leve2_count++;
                    printf("Level 2 Battery Count: %d\n",leve2_count);
                    if(leve2_count>10){
                        task_bar_status[9]=0x32;
                        low_bat_count=0;
                        leve1_count=0;
                        leve2_count=11;
                    } else {
                        task_bar_status[9]=0x33;
                        low_bat_count=0;
                        leve1_count=0;
                    }
                }
                else if(bat_Temp<=83 && bat_Temp>=80) {
                    leve1_count++;
                    printf("Level 1 Battery Count: %d\n",leve1_count);
                    if(leve1_count>10){
                        task_bar_status[9]=0x31;
                        low_bat_count=0;
                        leve1_count=11;
                    } else {
                        task_bar_status[9]=0x32;
                        low_bat_count=0;
                    }
                }
                else if(bat_Temp<= 79){
                    low_bat_count++;
                    printf("Level 0 Battery Count: %d\n",low_bat_count);
                    //task_bar_status[9]==0x37 //For shutdown low battery
                    if(low_bat_count>=10) {
                        task_bar_status[9]=0x38;  //Low Battery Print Notification
                        low_bat_count=11;
                        if(bat_Temp<=78) {
                            task_bar_status[9]=0x37; //For shutdown low battery
                            printf("Device shutting down\n");
                            //                        system("poweroff");
                        }
                    }else {
                        task_bar_status[9]=0x31;
                    }
                }
                tmp_bat = bat_Temp;
            }
        }

        printf("Charge Status: %c =============>\n", task_bar_status[9]);

        for (c = 0; c < 11; c++)
        {
            *s++ = task_bar_status[c];
        }
        *s = '\0';
        shmdt(shm);
        shmdt(s);
        for(i=0;i<11;i++)
        {
            printf("%c",task_bar_status[i]);
        }
        printf("\n");
        FILE *ftimesync;
        int timesync_status;
        if( access("/usr/share/status/timesync", F_OK ) != -1 ) {
            // file exists
            ftimesync = fopen("/usr/share/status/timesync","r");
            fscanf(ftimesync,"%d",&timesync_status);
            fclose(ftimesync);
            if(timesync_status==1){
                system("date +\"%s\" > /usr/share/status/timestamp");
                system("hwclock -w");
            }
        }
        if(system("ps | grep [n]etd > /dev/null")!=0){
            system("/opt/daemon_files/netd -d");
            sleep(1);
        }else if(system("ps | grep [s]tartApplication > /dev/null")!=0){
            system("sh /usr/share/scripts/bootapp.sh");
            sleep(1);
        }else{
            sleep(1);
        }
    }
}
