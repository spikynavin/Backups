#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

char *START = "start";
char *STOP = "stop";

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

void standby(int value)
{
    pid_t pid1 = proc_find("/usr/bin/background");
    pid_t pid2 = proc_find("/opt/daemon_files/netd");
    pid_t pid3 = proc_find("/opt/daemon_files/taskd");
    pid_t pid4 = proc_find("/lib/udev/udevd");
    pid_t pid5 = proc_find("/opt/daemon_files/gpiod");
    pid_t pid6 = proc_find("/opt/daemon_files/keyd");
    switch(value)
    {
        case 0:
            kill(pid1,SIGSTOP);
            kill(pid2,SIGSTOP);
            kill(pid3,SIGSTOP);
            kill(pid4,SIGSTOP);
            kill(pid5,SIGSTOP);
            kill(pid6,SIGSTOP);
        break;
        case 1:
            kill(pid1,SIGCONT);
            kill(pid2,SIGCONT);
            kill(pid3,SIGCONT);
            kill(pid4,SIGCONT);
            kill(pid5,SIGCONT);
            kill(pid6,SIGCONT);
        break;
    }
}


int main(int argc, char *argv[])
{
        //printf("Argument: %s",argv[1]);
        if (strcmp(argv[1], START) == 0)
        {
            //printf("Standby Start\n");
            standby(0);
        }
        else if (strcmp(argv[1], STOP) == 0)
        {
            //printf("Standby Stop\n");
            standby(1);
        }
        return 0;
}
