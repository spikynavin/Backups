#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <memory.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/signal.h>
#include <string.h>
#include <termios.h> 
#include <dirent.h>
#include <linux/poll.h>
#include <netdb.h>
#include <sys/signal.h>

FILE *fbluestat, *fblue, *fgprspwr, *fgprsen,*fpp, *fps,*fb, *fscan,*fstatus;
#define BAUDRATE B115200
int i,fdt,state_stat,state_ppp,k,con;
 char bton[13]="AT+QBTPWR=1\n",btoff[13]="AT+QBTPWR=0\n",btscan[15]="AT+QBTSCAN=1\n", btscanc[16]="AT+QBTSCANC\n",btstate[13]="AT+QBTSTATE\n",read_buffer[30],bscan_buff[10],gsmcsq[8]="AT+CSQ\n",status_buff[30];
 int mux_fd;

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

static void enable_gprs(void)
{
        fgprspwr = fopen("/sys/class/gpio/gpio42/value", "w");
        fwrite("1",1,2,fgprspwr);
        fclose(fgprspwr);
        //sleep(1);
	sleep(1);
        fgprsen = fopen("/sys/class/gpio/gpio288/value", "w");
        fwrite("1",1,2,fgprsen);
        fclose(fgprsen);
        sleep(1);
}
static void blue_en(void)
{
	printf("Test-1\n");

	struct termios oldtio,newtio;
        mux_fd = open("/dev/mux2", O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd, TCIFLUSH);
        tcsetattr(mux_fd,TCSANOW,&newtio);

        write(mux_fd,bton,sizeof(bton));

	printf("Test-2\n");	

        while(1)
	{
		printf("Test-3\n");

                read(mux_fd, read_buffer, sizeof(read_buffer));

		printf("String:%s\n",read_buffer);
		if(read_buffer[0]=='O')
		{
		printf("Test-4\n");		
		fb = fopen("/opt/daemon_files/bluestat", "w");
		fprintf(fb,"%s","B");
		fclose(fb);
                break;
		}
	memset(read_buffer,0,sizeof(read_buffer));

	}
        close(mux_fd);
}
static void signal_check(void)
{

	struct termios oldtio,newtio;
        mux_fd = open("/dev/mux2", O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd, TCIFLUSH);
        tcsetattr(mux_fd,TCSANOW,&newtio);

	printf("Signal check \n");
        write(mux_fd,gsmcsq,sizeof(gsmcsq));

        while(1)
	{
                read(mux_fd, read_buffer, sizeof(read_buffer));
//		printf("%s\n",read_buff);
		if(read_buffer[0]=='+' && read_buffer[1]=='C' && read_buffer[2]=='S' && read_buffer[3]=='Q')
		{
//                      printf("CSQ Received!!!\n");
			char sig[2];
			sig[0]=read_buffer[6];
			sig[1]=read_buffer[7];
                        sig[2]='\0';
                        FILE *fd_csq;
                        fd_csq = fopen("/opt/daemon_files/rough_files/signal_level","w");
                        fprintf(fd_csq,sig,2);
                        fclose(fd_csq);
			int sig_int=atoi(sig);
	          fdt = open("/opt/daemon_files/tower_value", O_RDWR | O_NOCTTY ); 
		   if(sig_int>5 && sig_int<=10)
			{
				write(fdt,"1",2);
			}
		   else if(sig_int>10 && sig_int<=15)
			{
				write(fdt,"2",2);
			}
		   else if(sig_int>15 && sig_int<=20)
			{
				write(fdt,"3",2);
			}
		   else if(sig_int>20 && sig_int<=25)
			{
				write(fdt,"4",2);
			}
		   else if(sig_int>25 && sig_int<=30)
			{
				write(fdt,"5",2);
			}
		close(fdt);
                break;
		}
		memset(read_buffer,0,sizeof(read_buffer));

	}
close(mux_fd);
}
static void bt_stat(void)
{	

	struct termios oldtio,newtio;
        mux_fd = open("/dev/mux2", O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd, TCIFLUSH);
        tcsetattr(mux_fd,TCSANOW,&newtio);

    fps = fopen("/opt/daemon_files/btpaired", "w");
    fprintf(fps,"%s","");
    fclose(fps);

   // write(fd,btstate,sizeof(btstate));
   write(mux_fd,btstate,sizeof(btstate));

    while(1)
    {
	printf("Blue State Scan\n");
        read(mux_fd, read_buffer, sizeof(read_buffer));

	if(read_buffer[0]=='+' && read_buffer[1]=='Q' && read_buffer[2] == 'B' && read_buffer[3]=='T' && read_buffer[4]=='S' && read_buffer[5]=='T')
	{
			fpp = fopen("/opt/daemon_files/btpaired", "a");
			fprintf(fpp,"%s",read_buffer);
			fclose(fpp);
	}
	else if(read_buffer[0]=='O')
	{
		printf("BLUETOOTH STATE CHECK DONE\n");
                break;
	}
	memset(read_buffer,0,sizeof(read_buffer));

    }
close(mux_fd);
}
static void bt_scan(void)
{
fscan = fopen("/opt/daemon_files/btscan", "r");
fread(bscan_buff,2,1,fscan);
fclose(fscan);
if(bscan_buff[0]=='1')
{
	printf("Do Scan...\n");

	struct termios oldtio,newtio;
        mux_fd = open("/dev/mux2", O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd, TCIFLUSH);
        tcsetattr(mux_fd,TCSANOW,&newtio);

    fps = fopen("/opt/daemon_files/btscan", "w");
    fprintf(fps,"%s","");
    fclose(fps);

    state_ppp=1;
   // write(fd,btscan,sizeof(btscan));
        write(mux_fd, btscan,sizeof(btscan));

    for(i=0;i<=10;i++)
    {
	printf("Blue device Scan\n");	
        read(mux_fd, read_buffer, sizeof(read_buffer));
		if(read_buffer[9]==' ')
		{
			fps = fopen("/opt/daemon_files/btscan", "a");
			fprintf(fps,"%s",read_buffer);
			fclose(fps);
		}
		else if(read_buffer[9]=='0')
		{
			break;
		}
	memset(read_buffer,0,sizeof(read_buffer));
    }
  close(mux_fd);
}
}
static void blue_dis(void)
{	

	struct termios oldtio,newtio;
        mux_fd = open("/dev/mux2", O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd, TCIFLUSH);
        tcsetattr(mux_fd,TCSANOW,&newtio);

        write(mux_fd,btoff,sizeof(btoff));

            write(mux_fd,btoff,sizeof(btoff));

        while(1)
	{

            read(mux_fd, read_buffer, sizeof(read_buffer));

		if(read_buffer[0]=='O')
		{		
		fb = fopen("/opt/daemon_files/bluestat", "w");
		fprintf(fb,"%s","0");
		fclose(fb);
                break;
		}
		memset(read_buffer,0,sizeof(read_buffer));

	}
close(mux_fd);
}
int main()
{
    fbluestat = fopen("/opt/daemon_files/bluestat", "w");
    fprintf(fbluestat,"%s","");
    fclose(fbluestat);

    fps = fopen("/opt/daemon_files/btscan", "w");
    fprintf(fps,"%s","");
    fclose(fps);

    fpp = fopen("/opt/daemon_files/btpaired", "w");
    fprintf(fpp,"%s","");
    fclose(fpp);

    char bstatus_buff[10],bstat_buff[2];
/*
   pid_t pid, sid;
   //Fork the Parent Process
    pid = fork();
    if (pid < 0) { exit(EXIT_FAILURE); }
    //We got a good pid, Close the Parent Process
    if (pid > 0) { exit(EXIT_SUCCESS); }
    //Change File Mask
    umask(0);
    //Create a new Signature Id for our child
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }
    //Change Directory
    //If we cant find the directory we exit with failure.
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
    //Close Standard File Descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

*/
//Bluetooth Enable Flow
while(1)
{
//    fstatus = fopen("/opt/daemon_files/nw_status", "r");
//    fread(status_buff, 10, 1, fstatus);
//    fclose(fstatus);
//    if(status_buff[5]=='2')
//    {
//           sleep(2);
//           signal_check();
//    }


fblue = fopen("/opt/daemon_files/blueen","r");
fread(bstatus_buff,2,1,fblue);
fclose(fblue);
printf("While/if Started...\n");
	if(bstatus_buff[0]=='B' && bstatus_buff[1]=='1')
		{
		pid_t pid = proc_find("gsmMuxd");
		if (pid == -1) 
		   {
			enable_gprs();
                        system("gsmMuxd -b 115200 -p /dev/ttyS3 -r -s /dev/mux /dev/ptmx /dev/ptmx /dev/ptmx");
                        sleep(1);
		   } 
		   else
		   {
			printf("Module Enabling\n");
			fbluestat = fopen("/opt/daemon_files/bluestat","r");
			fread(bstat_buff,2,1,fbluestat);
			fclose(fbluestat);
			if(bstat_buff[0]=='B')
			  {
                                sleep(1);
				bt_stat();
				bt_scan();
			  }
			else
			  {
				blue_en();
			  }
		   }	
		}
	else if(bstatus_buff[0]=='B' && bstatus_buff[1]=='0')
		{
			printf("Module Disabling\n");
			fbluestat = fopen("/opt/daemon_files/bluestat","r");
			fread(bstat_buff,2,1,fbluestat);
			fclose(fbluestat);
			if(bstat_buff[0]=='B')
			  {
				blue_dis();
			  }
			else
			  {

			  }
		}
        sleep(1);
}
return 0;	
}
