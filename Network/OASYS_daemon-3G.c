#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/ioctl.h>    // SIOCGIFFLAGS
#include <errno.h>        // errno
#include <netinet/in.h>   // IPPROTO_IP
#include <net/if.h>       // IFF_*, ifreq
#include <arpa/inet.h>
#include <sys/wait.h> 	  /* for wait */
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

#define GSMPORT "/dev/chn/1"
FILE *fbluestat, *fblue, *fgprs_pwr, *fgprs_en,*fpp, *fps,*fb, *fscan,*fstatus,*fping,*fsim,*fcops;
#define BAUDRATE B115200

int i,fdt,state_stat,state_ppp,k,con,sim_status,cops_status,creg_status,signal_status,ping_value,count=0;
 char bton[13]="AT+QBTPWR=1\n",btoff[13]="AT+QBTPWR=0\n",btscan[15]="AT+QBTSCAN=1\n", btscanc[16]="AT+QBTSCANC\n",btstate[13]="AT+QBTSTATE\n",read_buffer[30],bscan_buff[10];
char gsmcpin[10]="AT+CPIN?\n",gsmcsq[10]="AT+CSQ\n",gsmcops[10]="AT+COPS?\n",gsmcreg[10]="AT+CREG?\n",copcheck[12]="AT+COPS=0\n",atcheck[10]="AT\n",status_buff[30],cops[10],sim[10];
 struct pollfd mux_fd[1];

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
	fgprs_pwr = fopen("/sys/class/gpio/gpio65/value", "w");
        fwrite("1",1,2,fgprs_pwr);
	fclose(fgprs_pwr);
        sleep(1);
}
static void disable_gprs(void)
{
        fgprs_pwr = fopen("/sys/class/gpio/gpio65/value", "w");
        fwrite("0",1,2,fgprs_pwr);
	//sleep(1);
	//fwrite("1",1,2,fgprs_pwr);
        fclose(fgprs_pwr);
        //sleep(1);
	sleep(1);
       /* fgprs_en = fopen("/sys/class/gpio/gpio53/value", "w");
        fwrite("1",1,2,fgprs_en);
	sleep(1);
	fwrite("0",1,2,fgprs_en);	
        fclose(fgprs_en);
        sleep(1);*/
}

static void restart_ppp(void)
{
	//system("killall gsm0710muxd_bp > /dev/null");
	
	system("killall pppd > /dev/null");
}

static void sim_check(char* port)
{
	
	struct termios oldtio,newtio;
        mux_fd[0].fd = open(port, O_RDWR | O_NOCTTY );
	tcgetattr(mux_fd[0].fd,&oldtio); /* save current serial port settings */
   	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd[0].fd, TCIFLUSH);
        tcsetattr(mux_fd[0].fd,TCSANOW,&newtio);
	//printf("Sim check \n");
        //write(fd,gsmcsq,sizeof(gsmcsq));
        mux_fd[0].events = POLLOUT | POLLWRNORM;
        int retval2 = poll(mux_fd, 1, 1000);
        if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
            write(mux_fd[0].fd,gsmcpin,sizeof(gsmcpin));
	}
	int state_read = 1;
	
	while(state_read)
	{
		
                mux_fd[0].events = POLLIN | POLLRDNORM;
                int retval1 = poll(mux_fd, 1, 1000);
		
                if (retval1 && (mux_fd[0].revents & POLLRDNORM)) {
                read(mux_fd[0].fd, read_buffer, sizeof(read_buffer));
		}
		//printf("inside while %s\n",read_buffer);
		if(read_buffer[1]=='C' && read_buffer[2]=='P' && read_buffer[3]=='I' && read_buffer[4]=='N')
		{
		 //printf("SIM Received!!!\n");
			char sig[6];
			sig[0]=read_buffer[7];
			sig[1]=read_buffer[8];
			sig[2]=read_buffer[9];
			sig[3]=read_buffer[10];
			sig[4]=read_buffer[11];
                        sig[5]='\0';
		
			if(sig[0] == 'R' && sig[4] == 'Y')
			{
			FILE *fd_cpin;
			fd_cpin = fopen("/opt/daemon_files/rough_files/sim_presence","w");
                        fprintf(fd_cpin,sig,6);
			fclose(fd_cpin);
			sim_status=1;
			}
			else
			sim_status=0;
			
	
			state_read=0;	
		}
		
		//tcsetattr(mux_fd[0].fd,TCSANOW,&oldtio);
		memset(read_buffer,0,sizeof(read_buffer));
		}
	//	close(mux_fd[0].fd);
		
			//int sig_int=atoi(sig);
}


static void cops_check(char* port)
{
	/*struct termios oldtio,newtio;
        mux_fd[0].fd = open(port, O_RDWR | O_NOCTTY );
	printf("file:%d\n",mux_fd[0].fd);
        tcgetattr(mux_fd[0].fd,&oldtio); 
   	bzero(&newtio, sizeof(newtio)); 
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd[0].fd, TCIFLUSH);
        tcsetattr(mux_fd[0].fd,TCSANOW,&newtio);*/

	//printf("cops check \n");
        //write(fd,gsmcsq,sizeof(gsmcsq));
        mux_fd[0].events = POLLOUT | POLLWRNORM;
        int retval2 = poll(mux_fd, 1, 1000);
	//printf("cops checkedd2:%d\n",retval2);
        if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
            write(mux_fd[0].fd,gsmcops,sizeof(gsmcops));
        }
	int state_read =1;
	
	while(state_read)
	{
		
                mux_fd[0].events = POLLIN | POLLRDNORM;
                int retval1 = poll(mux_fd, 1, 1000);
		//printf("cops checkedd:%d\n",retval1);
                if (retval1 && (mux_fd[0].revents & POLLRDNORM)) {
                read(mux_fd[0].fd, read_buffer, sizeof(read_buffer));
		}
		
		if(read_buffer[1]=='C' && read_buffer[2]=='O' && read_buffer[3]=='P' && read_buffer[4]=='S')
		{
		 //printf("COPS Received!!!\n");
			char sig[6];
			sig[0]=read_buffer[7];
			sig[1]=read_buffer[9];
			/*sig[2]=read_buffer[10];
			sig[3]=read_buffer[11];
			sig[4]=read_buffer[12];
			sig[5]=read_buffer[12];
			sig[6]=read_buffer[12];
			sig[7]=read_buffer[12];
			sig[8]=read_buffer[12];
			sig[9]=read_buffer[12];*/
                        sig[5]='\0';
			//printf("cops:%s\n",read_buffer);
			if(sig[0] == '0' && sig[1] == '0')
			{
			
			FILE *fd_cops;
			fd_cops = fopen("/opt/daemon_files/rough_files/current_operator","w");
                        fprintf(fd_cops,read_buffer,sizeof(read_buffer));
			fclose(fd_cops);
			system("sh /opt/daemon_files/c_operator");
			cops_status=1;
			state_read=0;
			}
			else
			{
			//printf("signal111\n");
			memset(read_buffer,0,sizeof(read_buffer));
			mux_fd[0].events = POLLOUT | POLLWRNORM;
		        int retval2 = poll(mux_fd, 1, 1000);
			if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
		            write(mux_fd[0].fd,gsmcops,sizeof(gsmcops));
			//printf("signal\n");	
		        }
			cops_status=0;
			}		
				
		}
		memset(read_buffer,0,sizeof(read_buffer));
		}
		//int x=close(mux_fd[0].fd);	
		//printf("close state:%d\n",x);
}

static void cops_recheck(void)
{
	/*struct termios oldtio,newtio;
        mux_fd[0].fd = open(port, O_RDWR | O_NOCTTY );
	printf("file:%d\n",mux_fd[0].fd);
        tcgetattr(mux_fd[0].fd,&oldtio); 
   	bzero(&newtio, sizeof(newtio)); 
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd[0].fd, TCIFLUSH);
        tcsetattr(mux_fd[0].fd,TCSANOW,&newtio);*/

	//printf("cops recheck \n");
        //write(fd,gsmcsq,sizeof(gsmcsq));
        mux_fd[0].events = POLLOUT | POLLWRNORM;
        int retval2 = poll(mux_fd, 1, 1000);
	//printf("cops checkedd2:%d\n",retval2);
        if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
            write(mux_fd[0].fd,copcheck,sizeof(copcheck));
        }
	int state_read =1;
	
	while(state_read)
	{
		
                mux_fd[0].events = POLLIN | POLLRDNORM;
                int retval1 = poll(mux_fd, 1, 1000);
		//printf("cops checkedd:%d\n",retval1);
                if (retval1 && (mux_fd[0].revents & POLLRDNORM)) {
                read(mux_fd[0].fd, read_buffer, sizeof(read_buffer));
		}
		//printf("Cops_Rechecked\n");
		state_read=0;
	}				
		memset(read_buffer,0,sizeof(read_buffer));
	
		//int x=close(mux_fd[0].fd);	
		//printf("close state:%d\n",x);
}


static void signal_check(char* port)
{
	/*struct termios oldtio,newtio;
        mux_fd[0].fd = open(port, O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd[0].fd,&oldtio); 
   	bzero(&newtio, sizeof(newtio)); 
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd[0].fd, TCIFLUSH);
        tcsetattr(mux_fd[0].fd,TCSANOW,&newtio);*/

	//printf("Signal check \n");
        //write(fd,gsmcsq,sizeof(gsmcsq));
        mux_fd[0].events = POLLOUT | POLLWRNORM;
        int retval2 = poll(mux_fd, 1, 1000);
        if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
            write(mux_fd[0].fd,gsmcsq,sizeof(gsmcsq));
		
        }
	int state_read =1;
	while(state_read)
	{
                mux_fd[0].events = POLLIN | POLLRDNORM;
                int retval1 = poll(mux_fd, 1, 1000);
		//printf("csqqqqaswe\n");
                if (retval1 && (mux_fd[0].revents & POLLRDNORM)) {
		read(mux_fd[0].fd, read_buffer, sizeof(read_buffer));
		}
		//printf("TEST:%s\n",read_buffer);
		if(read_buffer[1]=='C' && read_buffer[2]=='S' && read_buffer[3]=='Q')
		{
                      //printf("CSQ Received!!!\n");
			/*char sig[2];
			sig[0]=read_buffer[6];
			sig[1]=read_buffer[7];
                        sig[2]='\0';*/
                        FILE *fd_csq;
                        fd_csq = fopen("/opt/daemon_files/rough_files/signal_level","w");
                        fprintf(fd_csq,read_buffer,sizeof(read_buffer));
                        fclose(fd_csq);
			system("sh /opt/daemon_files/tower");			

		/*	int sig_int=atoi(sig);
			fdt = open("/opt/daemon_files/tower_value", O_RDWR | O_NOCTTY ); 
		   if(sig_int>5 && sig_int<=10)
			{
				write(fdt,"1",2);
				printf("CSQ Received 1\n");
			}
		   else if(sig_int>10 && sig_int<=15)
			{
				write(fdt,"2",2);
				printf("CSQ Received 2\n");
			}
		   else if(sig_int>15 && sig_int<=20)
			{
				write(fdt,"3",2);
				printf("CSQ Received 3\n");
			}
		   else if(sig_int>20 && sig_int<=25)
			{
				write(fdt,"4",2);
				printf("CSQ Received 4\n");
			}
		   else if(sig_int>25 && sig_int<=30)
			{
				write(fdt,"5",2);
				printf("CSQ Received 5\n");
			}
		close(fdt);*/
		signal_status=1;
		state_read=0;	
		}
		else
		signal_status=0;

		memset(read_buffer,0,sizeof(read_buffer));
	}
//close(mux_fd[0].fd);
}

static void creg_check(char* port)
{
	/*struct termios oldtio,newtio;
        mux_fd[0].fd = open(port, O_RDWR | O_NOCTTY );
        tcgetattr(mux_fd[0].fd,&oldtio); 
   	bzero(&newtio, sizeof(newtio)); 
   	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
   	newtio.c_iflag = IGNPAR | ICRNL;
   	newtio.c_oflag = 0;
   	newtio.c_lflag = ICANON;
        tcflush(mux_fd[0].fd, TCIFLUSH);
        tcsetattr(mux_fd[0].fd,TCSANOW,&newtio);*/

	//printf("creg check \n");
        //write(fd,gsmcsq,sizeof(gsmcsq));
        mux_fd[0].events = POLLOUT | POLLWRNORM;
        int retval2 = poll(mux_fd, 1, 1000);
        if (retval2 && (mux_fd[0].revents & POLLWRNORM)) {
            write(mux_fd[0].fd,gsmcreg,sizeof(gsmcreg));
        }
	int state_read =1;
	
	while(state_read)
	{
		
                mux_fd[0].events = POLLIN | POLLRDNORM;
                int retval1 = poll(mux_fd, 1, 1000);
		if (retval1 && (mux_fd[0].revents & POLLRDNORM)) {
                read(mux_fd[0].fd, read_buffer, sizeof(read_buffer));
		}
		
		if(read_buffer[1]=='C' && read_buffer[2]=='R' && read_buffer[3]=='E' && read_buffer[4]=='G')
		{
		 //printf("CREG Received!!!\n");
			char sig[6];
			//sig[0]=read_buffer[7];
			sig[0]=read_buffer[9];
			/*sig[2]=read_buffer[10];
			sig[3]=read_buffer[11];
			sig[4]=read_buffer[12];
			sig[5]=read_buffer[12];
			sig[6]=read_buffer[12];
			sig[7]=read_buffer[12];
			sig[8]=read_buffer[12];
			sig[9]=read_buffer[12];*/
                        sig[5]='\0';
			//printf("creg:%s\n",read_buffer);
			if(sig[0] == '5')
			{
			//printf("creg\n");
			FILE *fd_creg;
			fd_creg = fopen("/opt/daemon_files/rough_files/creg","w");
                        fprintf(fd_creg,sig,6);
			fclose(fd_creg);
			creg_status=1;
			}
			else
			creg_status=0;
		
			state_read=0;	
		}
		memset(read_buffer,0,sizeof(read_buffer));
		}
		//close(mux_fd[0].fd);	

	
}	


static void cops_recheck_3G(void)
{
//printf("Cops rechecked\n");
system("sh /opt/daemon_files/GPRS_health.sh cops_recheck");
}

static void sim_check_3G(void)
{
system("sh /opt/daemon_files/GPRS_health.sh sim_presence");

	fsim=fopen("/opt/daemon_files/sim_value", "r");
	fread(sim,1,1,fsim);
	fclose(fsim);	
	if(sim[0]=='1')
	sim_status=1;
	//printf("Sim Checked\n");
}	

static void cops_check_3G(void)
{
system("sh /opt/daemon_files/GPRS_health.sh current_operator");

	fcops=fopen("/opt/daemon_files/rough_files/current_operator", "r");
        fread(cops,30,1,fcops);
        fclose(fcops);
	//printf("Cops_value:%s\n",cops);
	if(cops[7] == '0' && cops[9] == '0')	
	cops_status=1;
	else
	{
	//printf("Cops Not Checked\n");
	system("echo 1 > /opt/daemon_files/tower_value");
	system("echo nil > /opt/daemon_files/rough_files/signal_level");
	}
	
} 



static void signal_check_3G(void)
{
//printf("Signal Checking !!!\n");
system("sh /opt/daemon_files/GPRS_health.sh signal_level");
}

static void creg_check_3G(void)
{
//printf("Creg Checking !!!\n");
system("sh /opt/daemon_files/GPRS_health.sh creg");
}
static void restart_gsm(void)
{
	system("killall gsm0710muxd_bp > /dev/null");
	
	system("killall pppd > /dev/null");
}

static void ip_address(char* i_name)
{
    int n;
    struct ifreq ifr;
    n = socket(AF_INET, SOCK_DGRAM, 0);
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , i_name , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
    //display result
    FILE *fp;
    fp = fopen ("/opt/daemon_files/ip_address","w");
    fprintf(fp,"%s",inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));
    fclose(fp);
//    printf("IP Address is %s - %s\n" , i_name , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
}

static void ping(int a)
{
    if(a==1)
	{
	int state=1;	
		while(state)
    		{
			
    		if ( system("ping -c 2 8.8.8.8 -w 2 > /dev/null") == 0)
    		{
			sleep(1);
			fping = fopen("/opt/daemon_files/ping_status", "w");		
			fprintf(fping,"%s","G");
			fclose(fping);
        		//printf ("\n Exists");
			ping_value=1;
        		state=0;
			//printf("Pinging !!!\n");
    		}
		else if(system("ping -c 2 google.com -w 2 > /dev/null") == 0)
		{
			sleep(1);
			fping = fopen("/opt/daemon_files/ping_status", "w");		
			fprintf(fping,"%s","G");
			fclose(fping);
        		//printf ("\n Exists");
			ping_value=1;
        		state=0;
			//printf("Pinging in Idea Sim !!!\n");	
		}
    		else
    		{
			sleep(1);			
			fping = fopen("/opt/daemon_files/ping_status", "w");		
			fprintf(fping,"%s","g");
			fclose(fping);
			
			if(count == 2)
			{
		        ping_value=0;
			state=0;
			}	
			//printf("Not Pinging !!!\n");
			count++;
			if(count == 3)
			count=0;
       			//printf ("\n Not reachable ");
    		}
    		}
	}
}


static void gprs2g(void)
{
	
	while(1)
	{
	pid_t pid = proc_find("gsm0710muxd_bp");
	if (pid == -1) 
	{
	//printf("gprs enable\n");
	//enable_gprs();
	//system("gsm0710muxd_bp -s /dev/ttyO1 -b 115200 -n 3 -v 0 > /dev/null &");
	
	system("sh /opt/gprs_2g");
	//printf("gsmmux\n");
	sleep(1);
	} 
	else
	{
	sim_check("/dev/chn/1");

		if(sim_status == 1)
		cops_check("/dev/chn/1");
		else
		{
		//printf("SIM Not found\n");
		restart_gsm();
		disable_gprs();
		system("echo 20 > /opt/daemon_files/tower_value");
		}


		if(cops_status == 1)
		{
		signal_check("/dev/chn/1");
		creg_check("/dev/chn/1");
		pid_t ppp_pid = proc_find("pppd");
        	//printf("Pid PPPD:%d\n",ppp_pid);
        		if (ppp_pid == -1)
        		{
			//printf("pppd calling !!!!!!\n");
        		system("pppd call gprs2g");
        		}
        		else
        		{
			sleep(1);	
        		//printf("Pinging !!!\n");
        		ping(1);
			ip_address("ppp0");
        		}
		}
		
	}
	sleep(2);
	}


}

static void gprs3g(void)
{
	
	while(1)
	{
	
	sim_check_3G();

	if(sim_status == 1)
	cops_check_3G();
	else
	{
	//printf("Sim Not found\n");
	restart_ppp();
	disable_gprs();
	system("echo 20 > /opt/daemon_files/tower_value");
	}

	if(cops_status == 1)
	{
	signal_check_3G();
	creg_check_3G();
	pid_t ppp_pid = proc_find("pppd");
        //printf("Pid PPPD:%d\n",ppp_pid);
        	if (ppp_pid == -1)
        	{
		//printf("pppd calling !!!!!!\n");
       		system("pppd call gprs3g");
        	}
        	else
       		{
		sleep(1);	
        	//printf("Pinging !!!\n");
        	ping(1);
			if(cops_status == 1 && ping_value == 0)
			cops_recheck_3G();
			
        	ip_address("ppp0");
	      	}
	}
	sleep(2);
	}

	
}

/*int main()
{	
while(1)
{
if(gprs2G == 1)
	gprs2g();
	
	
sleep(2);
}
return 0;	
}*/

int main(int argc, char *argv[])
{
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

        FILE *fstatus;
        fstatus = fopen("/opt/daemon_files/nw_status", "r");
        fread(status_buff, 10, 1, fstatus);
        fclose(fstatus);

//printf("status buffer:%s",status_buff);

if(status_buff[5]=='2')
{
//	printf("2G Enabling !!!\n");
	
	restart_gsm();
	system("echo 0 > /opt/daemon_files/tower_value");
	system("echo 9 > /opt/daemon_files/ping_status");
	system("echo 0 > /opt/daemon_files/rough_files/creg");
        system("echo nil > /opt/daemon_files/rough_files/current_operator");
        system("echo nil > /opt/daemon_files/rough_files/signal_level");
	gprs2g();
}

if(status_buff[5]=='3')
{
	//printf("3G Enabling !!!\n");
	disable_gprs();
	restart_ppp();
	system("echo 0 > /opt/daemon_files/tower_value");
	system("echo 9 > /opt/daemon_files/ping_status");
	system("echo 0 > /opt/daemon_files/rough_files/creg");
        system("echo nil > /opt/daemon_files/rough_files/current_operator");
        system("echo nil > /opt/daemon_files/rough_files/signal_level");
	enable_gprs();
	sleep(15);
	gprs3g();
}	
closelog ();
}
