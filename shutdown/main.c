#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

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


int main(void)
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

    FILE *fp;
    unsigned long int data=0;
    fp = fopen("/opt/daemon_files/winid","w");
    fclose(fp);

    sleep(10);

    while(1)
    {
            pid_t process=proc_find("/usr/bin/shutdown");
            printf("Process ID: %d\n",process);
            if(process != -1)
            {
                system("export DISPLAY=:0.0;xdotool search --name \"shutdown\" > /opt/daemon_files/winid;");
            }

            fp = fopen("/opt/daemon_files/winid","r");
            fscanf(fp,"%ld",&data);
            fclose(fp);
            printf("Data:%ld\n",data);
            if(data>1000)
            {
                system("export DISPLAY=:0.0;var=`cat /opt/daemon_files/winid`;xdotool windowunmap $var;");
                break;
            }
            sleep(1);
    }

    return 0;
}

