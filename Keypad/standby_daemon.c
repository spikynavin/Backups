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

#define MOUSEFILE "/dev/input/event1"
#define HIDFILE    "/dev/hidraw0"
#ifndef EV_SYN
#define EV_SYN 0
#endif

unsigned int num=0;
unsigned int pwr=0;
char buf[100];
char back[100];
char buff[100];
int a;
char task_bar_status[1];
char *readbuff;
pthread_t tid, tid1;
int a_stat=0,shutdata=0,backlight_status=0, screenlock_status=0;
int n_stat,s_stat,b_stat;

volatile sig_atomic_t thread_stat = 0;
static timer_t tmid0, tmid1, tmid2;
unsigned int stdata=60;
int standby_status_check=0;

//    signal( SIGALRM, handle_alarm );
int r = EXIT_SUCCESS;
static void file_write(char *filename,char *data)
{
    int fp_write;
    fp_write = open(filename,O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if(fp_write){
        write(fp_write,data,sizeof(data));
        close(fp_write);
    }
}

void handle_alarm( int Sig, siginfo_t *Info, void *Ptr )
{
    FILE *bfile_timer, *stand_file_timer;
    if(Info->si_value.sival_ptr == &tmid0){
//        printf("Timer-1 Function Triggered\n");
        system("touch /usr/share/status/SCREEN_timeout");
//        system("sh /usr/share/scripts/Buzzer 1");
        //system("sh /opt/power_standby.sh 2> /dev/null");
        system("touch /usr/share/status/SCREEN_timeout;/bin/sh /usr/share/scripts/backlight 0");
        backlight_status=0;

        int bdata, sdata;
        bfile_timer = fopen("/usr/share/status/backlight_read_time","r");
        if(bfile_timer) {
            fscanf(bfile_timer,"%d",&bdata);
            fclose(bfile_timer);
        }
        if(bdata<0) {
            file_write("/usr/share/status/backlight_read_time","0");
            bdata=0;
        }
        stand_file_timer = fopen("/usr/share/status/standby_time","r");
        if(stand_file_timer) {
            fscanf(stand_file_timer,"%d",&sdata);
            fclose(stand_file_timer);
        }
        if(sdata<0) {
            file_write("/usr/share/status/standby_time","0");
            sdata=0;
        }
        timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={stdata,0} } , NULL);
//        printf("Buzzer Timer Set\n");
    }
    else if(Info->si_value.sival_ptr == &tmid1){
//        printf("Timer-2 Function Triggered\n");
        backlight_status=0;
        standby_status_check=1;
        /************************Reinitialize timer*************************/
        int bdata, sdata;
        bfile_timer = fopen("/usr/share/status/backlight_read_time","r");
        if(bfile_timer) {
            fscanf(bfile_timer,"%d",&bdata);
            fclose(bfile_timer);
        }
        stand_file_timer = fopen("/usr/share/status/standby_time","r");
        if(stand_file_timer) {
            fscanf(stand_file_timer,"%d",&sdata);
            fclose(stand_file_timer);
        }
        /************************Reinitialize timer*************************/
    }
    else if(Info->si_value.sival_ptr == &tmid2) {
        backlight_status=0;
//        printf("Timer-3 Function Triggered\n");
        FILE *os_standby, *app_standby;
        int standby_os_status=0, standby_app_status=0;
        app_standby = fopen("/usr/share/status/app_standby","r");
        fscanf(app_standby,"%d", &standby_app_status);
        fclose(app_standby);
//        printf("APP standby Status: %d\n", standby_app_status);

        char buffer[50];
        sprintf(buffer,"%d",get_avphys_pages() * getpagesize());
        int free_memory_int = atoi(buffer)/1000;
//        printf("free memory     : %d\n", free_memory_int);

        if(standby_status_check==1 && standby_app_status==1 && free_memory_int>5500){
//            printf("Standby Script Called\n");
            standby_status_check=0;
            backlight_status=0;
            system("echo 0 > /opt/daemon_files/standby_status");
            system("/opt/power_standby_scripts.sh > /dev/null &");
            sleep(1);

            int bdata, sdata;
            bfile_timer = fopen("/usr/share/status/backlight_read_time","r");
            if(bfile_timer) {
                fscanf(bfile_timer,"%d",&bdata);
                fclose(bfile_timer);
            }
            if(bdata<0) {
                file_write("/usr/share/status/backlight_read_time","0");
                bdata=0;
            }
            stand_file_timer = fopen("/usr/share/status/standby_time","r");
            if(stand_file_timer) {
                fscanf(stand_file_timer,"%d",&sdata);
                fclose(stand_file_timer);
            }
            if(sdata<0) {
                file_write("/usr/share/status/standby_time","0");
                sdata=0;
            }

            timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL);       // Resetting Screen Timeout with screentimeout file value
            timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={sdata,0} } , NULL);       // Resetting Standby Timeout with standbytimeout file value
            timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL);           // Resetting Buzzer Timeout to zero
        } else {
//            system("sh /usr/share/scripts/Buzzer 1");
            timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={stdata,0} } , NULL);
//            printf("Buzzer Timer Called\n");
        }
    }
out:
    if(r)
        perror(0);
}

/*********************************Pthread Job*********************************/

void* doSomeThing_standbyd(void *arg)
{
    pthread_t id = pthread_self();
    if(pthread_equal(id,tid))
    {
        FILE *bfile_standby;
        while(1){
//            printf("Thread Debug -1\n");
            int sfile_data=0;
            bfile_standby = fopen("/opt/daemon_files/standby_status", "r");
            if(bfile_standby)
            {
                fscanf(bfile_standby,"%d",&sfile_data);
                fclose(bfile_standby);
            }
            //            printf("Thread Debug -2\n");
            if(sfile_data==1)
            {
//                printf("Standby Thread Timer is Set\n");
                backlight_status=0;
                standby_status_check=1;
                timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL);
                timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL);
                timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={stdata,0} } , NULL);
                file_write("/opt/daemon_files/standby_status","0");
            }
            //            printf("Thread Debug -3\n");
            sleep(5);
        }
    }
}
//void Buzzer_setup(){
//        printf("Buzzer called\n");
//        system("echo 1 > /sys/class/gpio/gpio195/value");
//        usleep(1);
//        system("echo 0 > /sys/class/gpio/gpio195/value");
//}

/*********************************Pthread Job*********************************/


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
    } else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
    } else {
        printf("Debug Mode.\n");
    }

    FILE *keymode,*keysymbol, *bfile, *stand_file, *fptr, *ptback;
    int shmid,CAPS=0;
    key_t key;
    char *shm, *s;
    int home_press=0,alt_press=0,f4_press=0,homekey_press=0,power_press=0;

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

    //****************************** Backlight status *****************************//
    fptr = fopen("/usr/share/status/present_backlight","r");
    if(fptr){
        fscanf(fptr,"%d", &num);
//        printf("Backlight status= %d\n", num);
        snprintf(buf, sizeof(buf), "/bin/sh /usr/share/scripts/backlight %d",num);
        fclose(fptr);
    }
    system(buf);
    remove("/usr/share/status/CAPS_status");
    remove("/usr/share/status/CAPS_OFF_status");

    int bdata=0, sdata=0;

    bfile = fopen("/usr/share/status/backlight_read_time","r");
    if(bfile) {
        fscanf(bfile,"%d",&bdata);
        fclose(bfile);
    }

    stand_file = fopen("/usr/share/status/standby_time","r");
    if(stand_file) {
        fscanf(stand_file,"%d",&sdata);
        fclose(stand_file);
    }

    file_write("/opt/daemon_files/standby_status","0");
    timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL);
    timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={sdata,0} } , NULL);
    timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL);
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

    if(kdata==2)
    {
        int err = pthread_create(&(tid), NULL, &doSomeThing_standbyd, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");
        while (1) {
//            printf("Debug-1\n");
            n = read(fd, &ev, sizeof ev);
//            printf("Debug-2\n");
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
//            printf("Debug-3\n");
            int bdata=0;
            bfile = fopen("/usr/share/status/backlight_read_time","r");
            if(bfile)
            {
                fscanf(bfile,"%d",&bdata);
                fclose(bfile);
            }

//            printf("Debug-4\n");
//            printf("Backlight Timeout Data: %d\n", bdata);

            int standby_data=0;
            stand_file = fopen("/usr/share/status/standby_time","r");
            if(stand_file)
            {
                fscanf(stand_file,"%d",&standby_data);
                fclose(stand_file);
            }
//            printf("Debug-5\n");
//            printf("Standby Timeout Data: %d\n", standby_data);

            fptr = fopen("/usr/share/status/present_backlight","r");
            if(fptr){
                fscanf(fptr,"%d", &num);
//                printf("Backlight status= %d\n", num);
                snprintf(buf, sizeof(buf), "/bin/sh /usr/share/scripts/backlight %d",num);
                fclose(fptr);
            }
//            printf("Debug-6\n");
            if( access( "/usr/share/status/screen_lock", F_OK ) != -1 ) {
                screenlock_status=1;
                // file exists
            } else {
                // file doesn't exist
                screenlock_status=0;
            }
//            printf("Debug-7\n");
            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                timer_settime(tmid0, 0, &(struct  itimerspec const){ .it_value={bdata,0} } , NULL);
                timer_settime(tmid1, 0, &(struct  itimerspec const){ .it_value={standby_data,0} } , NULL);
                timer_settime(tmid2, 0, &(struct  itimerspec const){ .it_value={0,0} } , NULL);
//                printf("Debug-8\n");
                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    // system("sh /usr/share/scripts/backlight 3");
                    system(buf);
                    backlight_status=1;
                    standby_status_check=0;
                    //+5v Regulator
                    file_write("/sys/class/gpio/gpio43/value","1");
                    //USB Hub
                    file_write("/sys/class/gpio/gpio143/value","1");
                    usleep(5000);
                }
//                printf("Debug-9\n");

                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
                FILE *kf;
                kf = fopen("/usr/share/status/key_event", "w+");
                fprintf(kf,"%d", (int)ev.code);
                fclose(kf);
//                printf("Debug-10\n");
                if((int)ev.code==63) {
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
                } else if((int)ev.code==62){
                       switch(ev.value)
                    {
                    case 2:
                        f4_press=1;
                        break;
                    case 1:
                        f4_press=1;
                        break;
                    case 0:
                        f4_press=0;
                        break;
                    }
                }
//                printf("Debug-11\n");
//               if(((int)ev.code<=185 && ev.value==0)){
//                    system("echo 1 > /sys/class/gpio/gpio195/value");
//                    usleep(1);
//                    system("echo 0 > /sys/class/gpio/gpio195/value");
//                }
                if(((int)ev.code==102 && ev.value==0) && (home_press==1)){
                    system("echo 1 > /dev/fbrefresh0");
                    home_press=0;
                }
//                printf("Debug-12\n");
//                printf("HomePress=%d\n",home_press);
                if(((int)ev.code==2 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 1 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                }
                else if(((int)ev.code==3 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 2 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                }
                else if(((int)ev.code==4 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 3 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                }
                else if(((int)ev.code==5 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 4 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                }
                else if(((int)ev.code==7 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /usr/share/scripts/gadgets/serialgadget.sh start 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                } else if(((int)ev.code==11 && ev.value==0) && (home_press==1))
                {
                    //                    system("export DISPLAY=:0.0;sh /usr/share/scripts/gadgets/serialcomm.sh start 2> /dev/null &");
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 5 2> /dev/null &");
                    system("/usr/share/scripts/Buzzer 2");
                    home_press=0;
                }
                else if((home_press==1) && (f4_press==1) && screenlock_status==0){
                    system("killall Application;killall App_Utilities;killall App_Settings;killall TestTool;");
                    home_press=0;
                }
//                printf("Debug-13\n");
            }
        }
    }
    else if(kdata==3)
    {
        /*****************************Thread Creation*******************/
        //        int err = pthread_create(&(tid), NULL, &doSomeThing, NULL);
        //        if (err != 0)
        //            printf("\ncan't create thread :[%s]", strerror(err));
        //        else
        //            printf("\n Thread created successfully\n");

        //        pthread_join(tid);

        /*****************************Thread Creation*******************/
        int alpha_mode=0;
        while (1) {

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
//            printf("Backlight Timeout Data: %d\n", bdata);
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
            fptr = fopen("/usr/share/status/present_backlight","r");
            if(fptr){
                fscanf(fptr,"%d", &num);
//                printf("Backlight status= %d\n", num);
                snprintf(buf, sizeof(buf), "/bin/sh /usr/share/scripts/backlight %d",num);
                fclose(fptr);
            }
//            printf("Standby Timeout Data: %d\n", sdata);
            if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
            {
                printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
                alarm(0);
                alarm(bdata);
                /*Backlight Set*/
                if(backlight_status==0)
                {
                    //                    system("echo 90000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle");
                    // system("sh /usr/share/scripts/backlight 3");
                    system(buf);
                    backlight_status=1;
                    //+5v Regulator
                    file_write("/sys/class/gpio/gpio43/value","1");
                    //USB Hub
                    file_write("/sys/class/gpio/gpio143/value","1");
                    usleep(5000);
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
//                printf("HomePress=%d\n",home_press);
                if(((int)ev.code==2 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 1 2> /dev/null &");
//                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==3 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 2 2> /dev/null &");
//                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==4 && ev.value==0) && (home_press==1) )
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 3 2> /dev/null &");
//                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==5 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 4 2> /dev/null &");
//                    system("/usr/share/scripts/Buzzer 2");
                }
                else if(((int)ev.code==6 && ev.value==0) && (home_press==1))
                {
                    system("export DISPLAY=:0.0;sh /opt/sdk/resources/launcher.sh 5 2> /dev/null &");
//                    system("/usr/share/scripts/Buzzer 2");
                }
                if((int)ev.code==64 && (int)ev.value==1)
                {
                    system("echo 1 > /usr/share/status/KEYPAD_symbol");
                }
            }
        }
    }
    fflush(stdout);
    fprintf(stderr, "%s.\n", strerror(errno));
out:
    if(r)
        perror(0);
    return EXIT_FAILURE;
}
