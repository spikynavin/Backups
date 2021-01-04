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
#define HIDFILE    "/dev/hidraw0"
#ifndef EV_SYN
#define EV_SYN 0
#endif

FILE *bfile, *sfile, *shutfile, *stand_file;
FILE *bl, *vmf;

char task_bar_status[1];
pthread_t tid;
int a_stat=0,shutdata=0,backlight_status=0;
int n_stat,s_stat,b_stat,sdata=0;

volatile sig_atomic_t thread_stat = 0;
static timer_t tmid0, tmid1, tmid2;
unsigned int stdata=30;

//    signal( SIGALRM, handle_alarm );
int r = EXIT_SUCCESS;
static void file_write(char *filename,char *data)
{
    int fp_write;
    fp_write = open(filename,O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    write(fp_write,data,sizeof(data));
    close(fp_write);
}

void handle_alarm( int Sig, siginfo_t *Info, void *Ptr )
{
    FILE *fstatus;
    char status_buff[10];
    if(Info->si_value.sival_ptr == &tmid0){
        system("sh /usr/share/scripts/Buzzer 1");
        write(2, "tmid0\n", 6);
        system("export DISPLAY=:0.0;/usr/bin/xinput disable 6");
        //    printf("Alarm Called\n");
        //    system("echo 500 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
        system("sh /usr/share/scripts/backlight 0");
        //+5v Regulator
        file_write("/sys/class/gpio/gpio43/value","0");
        //USB Hub
        file_write("/sys/class/gpio/gpio143/value","0");
        backlight_status=0;
        if(timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={stdata,0} } , NULL) < 0)
        {
            r=EXIT_FAILURE;
        }
        printf("Buzzer Timer Set\n");
    }
    else if(Info->si_value.sival_ptr == &tmid1){
        write(2, "tmid1\n", 6);
        system("poweroff");
    }
    else if(Info->si_value.sival_ptr == &tmid2) {
        system("sh /usr/share/scripts/Buzzer 1");
        if(timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={stdata,0} } , NULL) < 0)
        {
            r=EXIT_FAILURE;
        }
        printf("Buzzer Timer Called\n");
    }
}

void check_status(void)
{
//    printf("Keypad Rotate\n");
    if( access( "/usr/share/status/Numeric_status", F_OK ) != -1) {
        task_bar_status[0]=0x33;
        remove("/usr/share/status/Numeric_status");
    }
    if( access( "/usr/share/status/Asmall_status", F_OK ) != -1 ) {
        task_bar_status[0]=0x31;
        remove("/usr/share/status/Asmall_status");
    }
    if( access( "/usr/share/status/Abig_status", F_OK ) != -1 ) {
        task_bar_status[0]=0x32;
        remove("/usr/share/status/Abig_status");
    }
}

void* doSomeThing_hid(void *arg)
{
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid))
    {
        printf("\n First thread accessing\n");
        int fd;
        struct input_event ie;
        while(1){
            if((fd = open(HIDFILE, O_RDONLY)) == -1) {
//                perror("opening device");
                //exit(EXIT_FAILURE);
                check_status();
                usleep(50000);
            } else {
                while(read(fd, &ie, sizeof(struct input_event))) {
                    printf("time %ld.%06ld\ttype %d\tcode %d\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);
                    int bdata;
                    bfile = fopen("/usr/share/status/backlight_read_time","r");
                    if(bfile) {
                        fscanf(bfile,"%d",&bdata);
                        fclose(bfile);
                    } if(bdata<0) {
                        file_write("/usr/share/status/backlight_read_time","0");
                        bdata=0;
                    }
                    //                    printf("Backlight Data: %d\n", bdata);

                    stand_file = fopen("/usr/share/status/standby_time","r");
                    if(stand_file) {
                        fscanf(stand_file,"%d",&sdata);
                        fclose(stand_file);
                    }
                    if(sdata<0) {
                        file_write("/usr/share/status/standby_time","0");
                        sdata=0;
                    }

                    if(0>timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL) )
                        system("exec /bin/sh /usr/share/scripts/backlight 4 &");
                    printf("Backlight reenabled\n");

                    usleep(50000);
                    system("export DISPLAY=:0.0;/usr/bin/xinput enable 6");
                    check_status();
                }
            }
        }
    }
    return NULL;
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

                    if(bdata<0)
                    {
                        file_write("/usr/share/status/backlight_read_time","0");
                        bdata=0;
                    }

                    stand_file = fopen("/usr/share/status/standby_time","r");
                    if(stand_file) {
                        fscanf(stand_file,"%d",&sdata);
                        fclose(stand_file);
                    }
                    if(sdata<0) {
                        file_write("/usr/share/status/standby_time","0");
                        sdata=0;
                    }

                    if(0>timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL) )
                        system("sh /usr/share/scripts/backlight 4");

                    usleep(5000);
                    system("export DISPLAY=:0.0;/usr/bin/xinput enable 6");
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

    /*Timer Initialization*/

    sigaction(SIGALRM, &(struct sigaction){ .sa_sigaction = handle_alarm, .sa_flags=SA_SIGINFO }, 0);
    printf("%p %p\n", (void*)&tmid0, (void*)&tmid1);

    struct sigevent sev = { .sigev_notify = SIGEV_SIGNAL, .sigev_signo = SIGALRM };

    sev.sigev_value.sival_ptr = &tmid0;
    if(0>timer_create(CLOCK_REALTIME,&sev,&tmid0))
    { r=EXIT_FAILURE; goto out; }

    sev.sigev_value.sival_ptr = &tmid1;
    if(0>timer_create(CLOCK_REALTIME,&sev,&tmid1))
    { r=EXIT_FAILURE; goto out; }

    sev.sigev_value.sival_ptr = &tmid2;
    if(0>timer_create(CLOCK_REALTIME,&sev,&tmid2))
    { r=EXIT_FAILURE; goto out; }

    /*Timer Initialization*/

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
    remove("/usr/share/status/CAPS_status");
    remove("/usr/share/status/CAPS_OFF_status");

    int bdata, kfile;

    bfile = fopen("/usr/share/status/backlight_read_time","r");
    if(bfile) {
        fscanf(bfile,"%d",&bdata);
        fclose(bfile);
    }
    if(bdata<0) {
        file_write("/usr/share/status/backlight_read_time","0");
        bdata=0;
    }
    stand_file = fopen("/usr/share/status/standby_time","r");
    if(stand_file) {
        fscanf(stand_file,"%d",&sdata);
        fclose(stand_file);
    }
    if(sdata<0) {
        file_write("/usr/share/status/standby_time","0");
        sdata=0;
    }

    if(0>timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL) )
    { r=EXIT_FAILURE; goto out; }
    if(0>timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={sdata,0} } , NULL) )
    { r=EXIT_FAILURE; goto out; }
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
        /*****************************Thread Creation*******************/
        int err = pthread_create(&(tid), NULL, &doSomeThing_hid, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

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
                if (errno == EINTR){
                    continue;
                }
                else
                {
                    break;
                }
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
            if(bdata<0)
            {
                file_write("/usr/share/status/backlight_read_time","0");
                bdata=0;
            }
            printf("Backlight Timeout Data: %d\n", bdata);
            stand_file = fopen("/usr/share/status/standby_time","r");
            if(stand_file)
            {
                fscanf(stand_file,"%d",&sdata);
                fclose(stand_file);
            }
            if(sdata<0)
            {
                file_write("/usr/share/status/standby_time","0");
                sdata=0;
            }
            printf("Standby Timeout Data: %d\n", sdata);
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

            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                if(0>timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL) )
                { r=EXIT_FAILURE; goto out; }
                if(0>timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={sdata,0} } , NULL) )
                { r=EXIT_FAILURE; goto out; }
                if(0>timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL) )
                { r=EXIT_FAILURE; goto out; }

//                system("sh /opt/power_standby.sh 0");

                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    system("sh /usr/share/scripts/backlight 4");
                    backlight_status=1;
                    //+5v Regulator
                    file_write("/sys/class/gpio/gpio43/value","1");
                    //USB Hub
                    file_write("/sys/class/gpio/gpio143/value","1");
                    usleep(5000);
                    system("export DISPLAY=:0.0;/usr/bin/xinput enable 6");
                }
                if(sfile==1)
                {
                    backlight_status=0;
                    system("echo 0 > /opt/daemon_files/standby_status");
                }

                //                standby(1);
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

                printf("Countk: %d\n", countk);

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
            if(bdata<0)
            {
                file_write("/usr/share/status/backlight_read_time","0");
                bdata=0;
            }
            printf("Backlight Timeout Data: %d\n", bdata);
            stand_file = fopen("/usr/share/status/standby_time","r");
            if(stand_file)
            {
                fscanf(stand_file,"%d",&sdata);
                fclose(stand_file);
            }
            if(sdata<0)
            {
                file_write("/usr/share/status/standby_time","0");
                sdata=0;
            }
            printf("Standby Timeout Data: %d\n", sdata);
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
                    backlight_status=1;
                    //+5v Regulator
                    file_write("/sys/class/gpio/gpio43/value","1");
                    //USB Hub
                    file_write("/sys/class/gpio/gpio143/value","1");
                    usleep(5000);
                    system("export DISPLAY=:0.0;/usr/bin/xinput enable 6");
                }
                //                standby(1);
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

out:
    if(r)
        perror(0);

    return EXIT_FAILURE;
}
