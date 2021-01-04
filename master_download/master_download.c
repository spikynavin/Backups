#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
FILE*fp,*fd;
char buf[1024];
int state;
char str[11];
int main(int argc, char *argv[])
{
    system("touch /home/root/is_download");
    system("touch /home/root/log_download");
    system("ln -s /home/root/log_download /home/root/log_download_read");
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
    while(1)
    {
        //printf("test 1 ===>>>\n");
        fp=fopen("/home/root/is_download","r");
        //printf("test 2 ===>>>\n");
        fscanf(fp,"%d",&state);
        //printf("test 3 ===>>>\n");
        //printf("File content:%d\n",state);
        //printf("test 4 ===>>>\n");
        if(fp==NULL){
            printf("State file not found\n");
        }else if(state == 1){
            //            fd=fopen("/home/root/db_name","r");
            //            fscanf(fd,"%s",buf);
            //            fclose(fd);
            //            //printf("buf File Content:%s\n",buf);
            //            strcat(str,buf);
            //            printf("Fail to bo downloaded: %s\n",str);
            system("rm /tmp/QT_MASTER_TNSTC.db");
            system("rm /home/root/status_download");
            system("rm /home/root/log_download");
            system("rm /home/root/status_download");
            system("sh /usr/share/scripts/Buzzer 1");
            system("sshpass -p 'Ana10Gi(s560094' rsync -avzP --progress --timeout=300 -e 'ssh -p 2020 -o ConnectTimeout=30 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no' analogics@115.160.248.114:/home/atm/PallavanDepotDBs/`cat /home/root/db_name` /home/root/`cat /home/root/db_name` > /home/root/log_download");
            system("echo 0 > /home/root/is_download");
            system("sh /home/root/after_download.sh");
            //system("sh /usr/share/scripts/Buzzer 4");
        }else
        {
            printf("State File found\n");
        }
        fclose(fp);
        usleep(50000);
    }

}
