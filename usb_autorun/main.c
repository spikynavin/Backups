#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

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

    pid_t installer;
    struct stat st = {0};
    if (stat("/media/thumbdrive", &st) == -1) {
        mkdir("/media/thumbdrive", 0700);
    }
    umount("/dev/sda1");
    mount("/dev/sda1","/media/thumbdrive","vfat",0, NULL);

    if( access("/media/thumbdrive/patch.clan", F_OK) != -1 ) {
        system("sh /usr/share/scripts/backlight 4");
        installer=fork();
        if(installer==0)
        {
            static char *argv[]={"clan_installer",NULL};
            execv("/usr/bin/clan_installer",argv);
        }
    } else if(access("/media/thumbdrive/application.clan", F_OK) != -1){
        system("sh /usr/share/scripts/backlight 4");
        installer=fork();
        if(installer==0) {
            static char *argv[]={"clan_installer",NULL};
            execv("/usr/bin/clan_installer",argv);
        }
    }
    return 0;
}

