#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHMSZ     27

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

    FILE *bfile;
    char task_bar_status[1], kfile[8];

    int shmid;
    key_t key;
    char *shm, *s;

    key = 3333;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    while(1){
        if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
            perror("shmat");
            exit(1);
        }
        s = shm;

        bfile = fopen("/proc/keypad/KEYPAD_mode","r");
        if(bfile)
        {
            fscanf(bfile,"%s",kfile);
            fclose(bfile);
        }
                printf("String: %s\n", kfile);
        if(strcmp(kfile,"num")==0){
            //            printf("Number\n");
            task_bar_status[0]='3';
        } else if(strcmp(kfile,"sml")==0) {
            //            printf("Small\n");
            task_bar_status[0]='1';
        } else if(strcmp(kfile,"big")==0) {
            //            printf("Big\n");
            task_bar_status[0]='2';
        }
        *s++ = task_bar_status[0];
        *s = '\0';
        shmdt(shm);
        shmdt(s);
        usleep(50000);
    }
    return 0;
}
