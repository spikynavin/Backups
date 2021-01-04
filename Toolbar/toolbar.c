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
#include <dirent.h>
#include <sys/signal.h>

#ifndef EV_SYN
#define EV_SYN 0
#endif
#define SHMSZ     27

unsigned int user_timing=0,value=0,backlight_dim=500;  // 500 --> 5 sec
unsigned int check_toolbar=0;
FILE *fp;

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

main()
{
    FILE *toolfile;
    int tdata;

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
        printf("checking toolbar.. %d\n",check_toolbar);
        if(check_toolbar==2)
        {
            printf("Inside check\n");
//            pid_t pid = proc_find("/usr/bin/toolbar");
//            printf("PID = %d\n",pid);
//            if(pid == -1)
//            {
//                printf("Toolbar not found\n");
//                system("exec /usr/bin/toolbar &");
//                sleep(5);
//            }

//            toolfile=fopen("/tmp/toolvalue","r");
//            fscanf(toolfile,"%d",&tdata);
//            fclose(toolfile);
//            if(tdata==0)
//            {
//                sleep(2);
//                toolfile=fopen("/tmp/toolvalue","r");
//                fscanf(toolfile,"%d",&tdata);
//                fclose(toolfile);

//                if(tdata==0)
//                {
//                    pid_t pidtool = proc_find("/usr/bin/toolbar");
//                    kill(pidtool,SIGKILL);
//                    system("exec /usr/bin/toolbar &");
//                    sleep(5);
//                }

//            }
            system("sh /opt/toolbar.sh");
            check_toolbar=0;
        }
        sleep(1);
    }
}

