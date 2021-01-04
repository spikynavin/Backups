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
#include <sys/time.h>
#include <time.h>

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
int deep_standby_state=0;

volatile sig_atomic_t thread_stat = 0;

static timer_t tmid0,tmid1;

static int file_read(char *filepath)
{
    FILE *ffile;
    int fdata;
    ffile = fopen(filepath,"r");
    if(ffile)
    {
        fscanf(ffile,"%d",&fdata);
        fclose(ffile);
    }
    return fdata;
}

void timer_alarm_set(int tim_no, int time)
{
    if(tim_no==0)
    {
        timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={time,0} } , NULL);
        //tmid0 expires after 1 second
    }
    else if(tim_no==1)
    {
        timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={time,0} } , NULL);
        //tmid1 expires after 3 seconds
    }
}

void handle_alarm( int Sig, siginfo_t *Info, void *Ptr )
{
    if(Info->si_value.sival_ptr == &tmid0)
    {
        printf("Timer 1 Called\n");
        //    system("echo 500 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
        system("sh /usr/share/scripts/backlight 0");
        system("echo 3 > /proc/sys/vm/drop_caches");
        backlight_status=0;
    }
    else if(Info->si_value.sival_ptr == &tmid1)
    {
        printf("Timer 2 Called\n");
        system("echo mem > /sys/power/state");
        deep_standby_state=1;
    }
}

/*********************************Pthread Job*********************************/

void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid))
    {
        //        printf("\n First thread processing\n");
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
                    int bdata = file_read("/usr/share/status/backlight_read_time");
                    int sdata = file_read("/usr/share/status/standby_time");
//                    int bdata;
//                    bfile = fopen("/usr/share/status/backlight_read_time","r");
//                    if(bfile)
//                    {
//                        fscanf(bfile,"%d",&bdata);
//                        fclose(bfile);
//                    }
                    system("sh /usr/share/scripts/backlight 4");

                    timer_alarm_set(0,0);
                    timer_alarm_set(0,bdata);

                    timer_alarm_set(1,0);
                    timer_alarm_set(1,sdata);
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


int main(void)
{
    int r = EXIT_SUCCESS;
    sigaction(SIGALRM, &(struct sigaction){ .sa_sigaction = handle_alarm, .sa_flags=SA_SIGINFO }, 0);
    printf("%p %p\n", (void*)&tmid0, (void*)&tmid1);

    struct sigevent sev = { .sigev_notify = SIGEV_SIGNAL, .sigev_signo = SIGALRM };

    sev.sigev_value.sival_ptr = &tmid0;
    timer_create(CLOCK_REALTIME,&sev,&tmid0);

    sev.sigev_value.sival_ptr = &tmid1;
    timer_create(CLOCK_REALTIME,&sev,&tmid1);


    FILE *keymode,*keysymbol;
    int shmid,CAPS=0;
    key_t key;
    char *shm, *s;

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

//    pid_t pid, sid;
//    pid = fork();
//    if (pid < 0) { exit(EXIT_FAILURE); }
//    if (pid > 0) { exit(EXIT_SUCCESS); }
//    umask(0);
//    sid = setsid();
//    if (sid < 0) { exit(EXIT_FAILURE); }
//    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
//    close(STDIN_FILENO);
//    close(STDOUT_FILENO);
//    close(STDERR_FILENO);

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

    system("sh /usr/share/scripts/backlight 4");

    int kfile;

    int bdata = file_read("/usr/share/status/backlight_read_time");
    int sdata = file_read("/usr/share/status/standby_time");
//    bfile = fopen("/usr/share/status/backlight_read_time","r");
//    if(bfile)
//    {
//        fscanf(bfile,"%d",&bdata);
//        fclose(bfile);
//    }

    timer_alarm_set(0,0);
    timer_alarm_set(0,bdata);

    timer_alarm_set(1,0);
    timer_alarm_set(1,sdata);

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

    system("echo 0 > /proc/keypad/KEYPAD_mode");
    system("echo 0 > /usr/share/status/KEYPAD_mode");

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

            int bdata = file_read("/usr/share/status/backlight_read_time");
            int sdata = file_read("/usr/share/status/standby_time");
            int sfile = file_read("/opt/daemon_files/standby_status");
            int kfile = file_read("/usr/share/status/KEYPAD_mode");

//            bfile = fopen("/usr/share/status/backlight_read_time","r");
//            if(bfile)
//            {
//                fscanf(bfile,"%d",&bdata);
//                fclose(bfile);
//            }


//            bfile = fopen("/opt/daemon_files/standby_status", "r");
//            if(bfile)
//            {
//                fscanf(bfile,"%d",&sfile);
//                fclose(bfile);
//            }

//            bfile = fopen("/usr/share/status/KEYPAD_mode","r");
//            if(bfile)
//            {
//                fscanf(bfile,"%d",&kfile);
//                fclose(bfile);
//            }

            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                timer_alarm_set(0,0);
                timer_alarm_set(0,bdata);

                timer_alarm_set(1,0);
                timer_alarm_set(1,sdata);
                //system("sh /usr/share/scripts/backlight 4");

                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    system("sh /usr/share/scripts/backlight 4");
                    backlight_status=1;
                    if(deep_standby_state==1)
                    {
                        system("sh /usr/share/scripts/bootapp.sh");
                        deep_standby_state=0;
                    }
                }
                if(sfile==1)
                {
                    backlight_status=0;
                    system("echo 0 > /opt/daemon_files/standby_status");
                }

                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
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
                    else if((int)ev.value==0 && kfile != 0)
                    {
                        if(kfile != 0)
                        {
                            system("echo 0 > /proc/keypad/KEYPAD_mode");
                            system("echo 0 > /usr/share/status/KEYPAD_mode");
                            task_bar_status[0]=0x33;
                            countk=0;
                            printf("Numeric mode set reset\n");
                        }
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
                timer_alarm_set(0,0);
                timer_alarm_set(0,bdata);

                timer_alarm_set(1,0);
                timer_alarm_set(1,sdata);
                /*Backlight Set*/
                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    system("sh /usr/share/scripts/backlight 4");
                    backlight_status=1;
                }
                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
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
                        }
                        else if(num_stat[0]=='0')
                        {
                            task_bar_status[0]=0x33;
                            num_stat[0]='1';
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
                else if((int)ev.code==58 && (int)ev.value==1)
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
