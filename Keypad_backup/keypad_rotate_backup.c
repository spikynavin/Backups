#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>

#define SHMSZ     27
#define MOUSEFILE "/dev/input/event1"
#ifndef EV_SYN
#define EV_SYN 0
#endif

FILE *bfile, *sfile, *shutfile;
FILE *bl, *vmf;

char task_bar_status[1];
pthread_t tid;
int a_stat=0,shutdata=0,backlight_status=0;
int n_stat,s_stat,b_stat;

volatile sig_atomic_t thread_stat = 0;

void standby(int value)
{
    //    switch(value)
    //    {
    //    case 0:
    //        system("/opt/daemon_files/standby.sh start");
    //        break;
    //    case 1:
    //        system("/opt/daemon_files/standby.sh stop");
    //        break;
    //    }
}

void handle_alarm( int sig )
{
    //    printf("Alarm Called\n");
    system("export DISPLAY=:0.0;exec /usr/bin/xinput disable 6");
    standby(0);
    //    system("echo 500 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    system("sh /usr/share/scripts/backlight 1");
    backlight_status=0;
}

/*********************************Pthread Job*********************************/

void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid))
    {
        //        printf("\n First thread ccessing\n");
        int fd;
        struct input_event ie;

        if((fd = open(MOUSEFILE, O_RDONLY)) == -1) {
            perror("opening device");
            //exit(EXIT_FAILURE);
        }
        else
        {
            while(read(fd, &ie, sizeof(struct input_event))) {
                printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);
                if(ie.code==330)
                {
                    int bdata;
                    bfile = fopen("/usr/share/status/backlight_read_time","r");
                    if(bfile)
                    {
                        fscanf(bfile,"%d",&bdata);
                        fclose(bfile);
                    }
                    system("sh /usr/share/scripts/backlight 4");
                    system("echo 1 > /sys/class/gpio/gpio143/value");
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    //system("sh /opt/daemon_files/standby.sh stop");
                    standby(1);
                    alarm(0);
                    alarm(bdata);
                    usleep(5000);
                    system("sh /opt/power_standby.sh 0");
                    system("export DISPLAY=:0.0;exec /usr/bin/xinput enable 6");

                }

            }
        }

    }

    return NULL;
}

/*********************************Pthread Job*********************************/

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


static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};

char standbydata[1];

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

    FILE *keymode,*keysymbol;
    int shmid,CAPS=0;
    key_t key;
    char *shm, *s;
    int home_press=0;

    key = 3333;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;

    signal( SIGALRM, handle_alarm );

    const char *dev = "/dev/input/event0";
    struct input_event ev;
    ssize_t n;
    int fd,countk=0;
    char num_stat[1],alp_stat[1];
    num_stat[0]='1';
    alp_stat[0]='1';
    task_bar_status[0]='3';
    *s++ = task_bar_status[0];
    *s = '\0';

    //    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
    system("sh /usr/share/scripts/backlight 4");
    system("echo 1 > /sys/class/gpio/gpio143/value");
    system("sh /opt/power_standby.sh 0");

    int bdata, kfile;

    bfile = fopen("/usr/share/status/backlight_read_time","r");
    if(bfile)
    {
        fscanf(bfile,"%d",&bdata);
        fclose(bfile);
    }

    alarm(0);
    alarm(bdata);
    usleep(5000);
    system("export DISPLAY=:0.0;exec /usr/bin/xinput enable 6");

    a_stat=1;

    fd = open(dev, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", dev, strerror(errno));
        return EXIT_FAILURE;
    }

    FILE *key_config;
    int kdata;
    key_config = fopen("/usr/share/status/KeyConfig","r");
    if(key_config)
    {
        fscanf(key_config,"%d",&kdata);
        fclose(key_config);
    }

    system("echo 0 > /proc/keypad/KEYPAD_mode; echo 0 > /usr/share/status/KEYPAD_mode");



    if(kdata==2)
    {
        while (1) {
            if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
                perror("shmat");
                exit(1);
            }
            s = shm;

            //task_bar_status[0]='1'; //Small
            //task_bar_status[0]='2'; //Caps
            //task_bar_status[0]='3'; //nums

            n = read(fd, &ev, sizeof ev);
            if (n == (ssize_t)-1) {
                if (errno == EINTR)
                    continue;
                else
                    break;
            } else
                if (n != sizeof ev) {
                    errno = EIO;
                    break;
                }

            int bdata;
            bfile = fopen("/usr/share/status/backlight_read_time","r");
            if(bfile)
            {
                fscanf(bfile,"%d",&bdata);
                fclose(bfile);
            }

            int sfile;

            bfile = fopen("/opt/daemon_files/standby_status", "r");
            if(bfile)
            {
                fscanf(bfile,"%d",&sfile);
                fclose(bfile);
            }

            bfile = fopen("/usr/share/status/KEYPAD_mode","r");
            if(bfile)
            {
                fscanf(bfile,"%d",&kfile);
                fclose(bfile);
            }

            if( access( "/usr/share/status/CAPS_status", F_OK ) != -1 ) {
                    task_bar_status[0]=0x32;
                    system("rm /usr/share/status/CAPS_status; echo 2 > /usr/share/status/KEYPAD_mode;");
            }
            if( access( "/usr/share/status/CAPS_OFF_status", F_OK ) != -1 ) {
                    task_bar_status[0]=0x31;
                    system("rm /usr/share/status/CAPS_OFF_status; echo 1 > /usr/share/status/KEYPAD_mode;");
            }
            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                alarm(0);
                alarm(bdata);

                //system("sh /usr/share/scripts/backlight 4");

                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    system("sh /usr/share/scripts/backlight 4");
                    system("echo 1 > /sys/class/gpio/gpio143/value");
                    backlight_status=1;
                    usleep(5000);
                    system("sh /opt/power_standby.sh 0");
                    system("export DISPLAY=:0.0; exec /usr/bin/xinput enable 6");
                    printf("Keypad lock disabled\n");
                }

                if(sfile==1)
                {
                    backlight_status=0;
                    system("echo 0 > /opt/daemon_files/standby_status");
                }

                standby(1);
                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);

                if((int)ev.code==63)
                {
                    switch(ev.value)
                    {
                    case 2:
                        home_press=1;
                        break;
                    case 1:
                        home_press=1;
                        break;
                    case 0:
                        home_press=0;
                        break;
                    }
                }

                if((int)ev.code==58 && (int)ev.value==1)
                {
                    if(CAPS==0)
                    {
                        CAPS=1;
                    }
                    else if(CAPS==1)
                    {
                        CAPS=0;
                    }
                }

                if((int)ev.code==56)
                {
                    printf("Kfile: %d\n", kfile);
                    if((int)ev.value==1 && kfile == 0)
                    {
                        if(countk==0)
                        {
                            //                        printf("Small set\n");
                            if(CAPS==0)
                            {
                                task_bar_status[0]=0x31;
                                printf("Small Alpha mode set rotate\n");
                                countk=1;
                            }
                            else if(CAPS==1)
                            {
                                task_bar_status[0]=0x32;
                                printf("Small Alpha mode set rotate\n");
                                countk=1;
                            }
                        }
                        else if(countk==1)
                        {
                            //                        printf("Num set\n");
                            task_bar_status[0]=0x33;
                            printf("Numeric mode set rotate\n");
                            countk=0;
                        }
                    }
                    else if((int)ev.value==1 && kfile != 0)
                    {
                        system("echo 0 > /proc/keypad/KEYPAD_mode");
                        system("echo 0 > /usr/share/status/KEYPAD_mode");
                        task_bar_status[0]=0x33;
                        countk=0;
                        printf("Numeric mode set reset\n");
                    }
                }

                if((int)ev.code==64 && (int)ev.value==1)
                {
                    system("echo 1 > /usr/share/status/KEYPAD_symbol");
                }

                printf("HomePress=%d\n",home_press);
                if(((int)ev.code==2 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 1 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==3 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 2 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==4 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 3 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==5 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 4 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==6 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 5 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }

            }
            *s++ = task_bar_status[0];
            *s = '\0';
            shmdt(shm);
            shmdt(s);
        }
    }
    else if(kdata==3)
    {
        /*****************************Thread Creation*******************/
        int err = pthread_create(&(tid), NULL, &doSomeThing, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

        //        pthread_join(tid);

        /*****************************Thread Creation*******************/
        int alpha_mode=0;
        while (1) {

            if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
                perror("shmat");
                exit(1);
            }
            s = shm;

            //task_bar_status[0]='1'; //Small
            //task_bar_status[0]='2'; //Caps
            //task_bar_status[0]='3'; //nums

            n = read(fd, &ev, sizeof ev);
            if (n == (ssize_t)-1) {
                if (errno == EINTR)
                    continue;
                else
                    break;
            } else
                if (n != sizeof ev) {
                    errno = EIO;
                    break;
                }

            int bdata;
            bfile = fopen("/usr/share/status/backlight_read_time","r");
            if(bfile)
            {
                fscanf(bfile,"%d",&bdata);
                fclose(bfile);
            }

            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
                alarm(0);
                alarm(bdata);

                /*Backlight Set*/
                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    system("sh /usr/share/scripts/backlight 4");
                    system("echo 1 > /sys/class/gpio/gpio143/value");
                    backlight_status=1;
                    usleep(5000);
                    system("sh /opt/power_standby.sh 0");
                    system("export DISPLAY=:0.0;exec /usr/bin/xinput enable 6");
                }
                standby(1);
                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);

                if((int)ev.code==58)
                {
                    switch(ev.value)
                    {
                    case 2:
                        home_press=1;
                        break;
                    case 1:
                        home_press=1;
                        break;
                    case 0:
                        home_press=0;
                        break;
                    }
                }
                printf("HomePress=%d\n",home_press);
                if(((int)ev.code==2 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 1 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==3 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 2 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==4 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 3 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==5 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 4 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==6 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 5 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                }

                if((int)ev.code==56)
                {
                    bfile = fopen("/usr/share/status/KEYPAD_mode","r");
                    if(bfile)
                    {
                        fscanf(bfile,"%d",&kfile);
                        fclose(bfile);
                    }
                    if((int)ev.value==1 && kfile == 0)
                    {
                        if(num_stat[0]=='1')
                        {
                            task_bar_status[0]=alp_stat[0];
                            num_stat[0]='0';
                            alpha_mode=1;
                        }
                        else if(num_stat[0]=='0')
                        {
                            task_bar_status[0]=0x33;
                            num_stat[0]='1';
                            alpha_mode=0;
                        }
                    }
                    else if((int)ev.value==0 && kfile != 0)
                    {
                        if(kfile != 0)
                        {
                            system("echo 0 > /proc/keypad/KEYPAD_mode");
                            system("echo 0 > /usr/share/status/KEYPAD_mode");
                            task_bar_status[0]=0x33;
                            num_stat[0]='1';
                            printf("Numeric mode set reset\n");
                        }
                    }
                }
                else if((int)ev.code==58 && alpha_mode==1 && (int)ev.value==1)
                {
                    if(alp_stat[0]=='1')
                    {
                        task_bar_status[0]=0x32;
                        alp_stat[0]='2';
                    }
                    else if(alp_stat[0]=='2')
                    {
                        task_bar_status[0]=0x31;
                        alp_stat[0]='1';
                    }
                }
                if((int)ev.code==64 && (int)ev.value==1)
                {
                    system("echo 1 > /usr/share/status/KEYPAD_symbol");
                }
            }
            *s++ = task_bar_status[0];
            *s = '\0';
            shmdt(shm);
            shmdt(s);
        }
    }
    fflush(stdout);
    fprintf(stderr, "%s.\n", strerror(errno));
    return EXIT_FAILURE;
}
