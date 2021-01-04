//++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                                  //
// This daemon file used to control the gpio's      //
//                                                  //
//++++++++++++++++++++++++++++++++++++++++++++++++++//

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
#include <sys/wait.h> /* for wait */
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ     27
#define ERROR(fmt, ...) do { printf(fmt, __VA_ARGS__); return -1; } while(0)

#define audio_power 498
#define camera_audio_switch 508
#define barcode_power 499
#define barcode_trigger 509
#define camera_power 500
#define fingerprint_power 511
#define magneticcard_power 114 
#define rfid_power 505
#define sam_power 503
#define smart_power 510
#define gps_power 496
//#define five_volt_per 46
#define usb_hub_power 502

struct ifreq ifr;
struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;

char WAITING[8] = "WAITING";
char SUCCESS[8] = "SUCCESS";
char FAILURE[8] = "FAILURE";

char status_buff[100],gprs_power[2],wifi_power[2];
int v5=0,usb_hub=0,fp=0,magnetic=0,rfid=0,sam=0,smart=0,camera=0,bar=0,audio=0,gps=0,idel=0;
char module_on_off[20];

FILE *fp_remote_modules;
FILE *fgprs_power;
FILE *fwifi_power;

int i=0;

//------------ GPIO access------------------------

void gpio(int pin_no,char gpiovalue)
{
	char start[100]="/sys/class/gpio/gpio";
	char end[100]="/value";
	char gpio_pin[5];
	char value[5];
	sprintf(gpio_pin, "%d", pin_no);
	strcat(start,gpio_pin);
	strcat(start,end);
//	printf("%s\n",start);
	FILE *fgpio;
//	printf("%c\n",gpiovalue);
	fgpio=fopen(start, "w");
	fwrite(&gpiovalue,1,1,fgpio);
	fclose(fgpio);
//	printf("gpio_fixed\n");
}


int main(int argc, char *argv[])
{
	int c;
	int shmid;
	key_t key;
	char *shm, *s;

	key = 3456;

	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}

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



	s = shm;
	for (c = 0; c <= 7; c++)
	*s++ = WAITING[c];
	*s = '\0';
//	printf("send\n");


	//----------------
	//Main Process
	//----------------

	while(1)
	{
	    while (*shm != '^')
		sleep(1);

//	printf("received.......\n");

	int i=0;

	for (s = shm; *s != '\0'; s++)
	{
		printf("%c",status_buff[i]);
		status_buff[i]=*s;
		i++;
	}

int iii=0;

for(iii=0;iii<7;iii++)
{
printf("%c",status_buff[iii]);
}

	printf("\n");


	// ---------------Success -------------------

	if(status_buff[0]=='^' && status_buff[6]=='!')
	{

		// ---------------- Audio (^001A1!) --------------------

		if(status_buff[2]=='0' && status_buff[3]=='1' && status_buff[4]=='A')
		{
			if(status_buff[5]=='1')
			{
				if(audio==0)
				{
				//	v5++;
					usb_hub++;
					gpio(audio_power,'1');
					gpio(camera_audio_switch,'1');
					audio=1;
				}

				else {

					if(audio!=0 && camera==1)
					gpio(camera_audio_switch,'1');
					
				     }	
			}
			else
			{
				if(audio!=0)
				{
				//	v5--;
					usb_hub--;
					gpio(audio_power,'0');
					gpio(camera_audio_switch,'0');
					audio=0;
				}
			}
		}
		// ---------------- Barcode (^002B1!) --------------------

		else if(status_buff[2]=='0' && status_buff[3]=='2' && status_buff[4]=='B')
		{
			if(status_buff[5]=='1')
			{
				if(bar==0)
				{
				//	v5++;
//					printf("barcode enable\n");
					gpio(barcode_trigger,'1');
					gpio(barcode_power,'1');
					bar=1;
				}
			}
			else if(status_buff[5]=='2')  // 2 - Bar code reader trigger Trigger
			{
					gpio(barcode_trigger,'0');
//					printf("barcode trigger\n");
			}
			else if(status_buff[5]=='3')  // 2 - Bar code reader trigger Trigger
			{
					gpio(barcode_trigger,'1');
//					printf("barcode trigger\n");
			}
			else
			{
				if(bar!=0)
				{
				//	v5--;
//					printf("barcode disable\n");
					gpio(barcode_trigger,'1');
					gpio(barcode_power,'0');
					bar=0;
				}
			}

		}
		// ---------------- Camera (^003C1!) --------------------

		else if(status_buff[2]=='0' && status_buff[3]=='3' && status_buff[4]=='C')
		{
			if(status_buff[5]=='1')
			{
				if(camera==0)
				{
				//	v5++;
					usb_hub++;
					gpio(camera_power,'1');
					gpio(camera_audio_switch,'0');
					sleep(2);
					camera=1;
				}
				else {
                                        if(camera!=0 && audio==1)
                                        gpio(camera_audio_switch,'0');
                                     }

			}
			else
			{
				if(camera!=0)
				{
				//	v5--;
					usb_hub--;
					gpio(camera_power,'0');
					camera=0;
				}
			}
		}
		// ---------------- Fingerprint (^004F1!) --------------------

		else if(status_buff[2]=='0' && status_buff[3]=='4' && status_buff[4]=='F')
		{
			if(status_buff[5]=='1')
			{
				if(fp==0)
				{
				//	v5++;
					usb_hub++;
					gpio(fingerprint_power,'1');
					fp=1;
				}
			}
			else
			{
				if(fp!=0)
				{
				//	v5--;
					usb_hub--;
					gpio(fingerprint_power,'0');
					fp=0;
				}
			}
		}

		// ---------------- GPS (^005G1!) --------------------

		else if(status_buff[2]=='0' && status_buff[3]=='5' && status_buff[4]=='G')
		{
			if(status_buff[5]=='1')
			{
				if(gps==0)
				{
				//	v5++;
					gpio(gps_power,'1');
					gps=1;
				}
			}
			else
			{
				if(gps!=0)
				{
				//	v5--;
					gpio(gps_power,'0');
					gps=0;
				}
			}
		}

		// ---------------- RFID (^006R1!) --------------------

		if(status_buff[2]=='0' && status_buff[3]=='6' && status_buff[4]=='R')
		{
			if(status_buff[5]=='1')
			{
				if(rfid==0)
				{
					gpio(barcode_power,'0');
				//	v5++;
					gpio(rfid_power,'1');
					rfid=1;
				}
			}
			else
			{
				if(rfid!=0)
				{
				//	v5--;
					gpio(rfid_power,'0');
					rfid=0;
				}
			}
		}


		// ---------------- SAM (^008S1!) --------------------

		if(status_buff[2]=='0' && status_buff[3]=='8' && status_buff[4]=='S')
		{
			if(status_buff[5]=='1')
			{
				if(sam==0)
				{
					
				//	v5++;
					gpio(sam_power,'1');
					sam=1;
				}
			}
			else
			{
				if(sam!=0)
				{
				//	v5--;
					gpio(sam_power,'0');
					sam=0;
				}
			}
		}



/*		// ---------------- IDEL (^010I1!) --------------------

		else if(status_buff[2]=='1' && status_buff[3]=='0' && status_buff[4]=='I')
		{
			if(status_buff[5]=='1')
			{
				idel=1;
				system("killall -19 net");
				system("sh /usr/share/scripts/backlight 0");
			}
			else
			{
				idel=0;
				system("killall -18 net");
				system("sh /usr/share/scripts/backlight 3");
			}
		}
*/
		// --------------- WIFI/GPRS POWER CHECKING ---------
/*
		fgprs_power=fopen("/sys/class/gpio/gpio65/value","r");
		fread(gprs_power,1,1,fgprs_power);
		fclose(fgprs_power);

		fwifi_power=fopen("/sys/class/gpio/gpio504/value","r");
		fread(wifi_power,1,1,fwifi_power);
		fclose(fwifi_power);
*/
		// --------------- 5V_per --------------------------

		if(v5<=0)
		{
//			if(gprs_power[0]=='1' || wifi_power[0]=='1')
//			{
//				if(idel==0)
//				{
//					gpio(five_volt_per,'1');
//				}
//				else
//				{
				//	gpio(five_volt_per,'0');
//				}
//			}
//			else
//			{
//				gpio(five_volt_per,'0');
//			}
		}
		else
		{
//			if(idel==0)
//			{
			//	gpio(five_volt_per,'1');
//			}
//			else
//			{
//				gpio(five_volt_per,'0');
//			}
		}

		// --------------- usb_hub --------------------------

		if(usb_hub<=0)
		{
//			if(wifi_power[0]=='1')
//			{
//				gpio(usb_hub_power,'1');
//			}
//			else
//			{
	//			gpio(usb_hub_power,'0');
//			}
		}
		else
		{
			gpio(usb_hub_power,'1');
		}

		s = shm;
		for (c = 0; c <= 7; c++)
		*s++ = SUCCESS[c];
		*s = '\0';
		
		}

		// ------------Failure --------------

		else
		{
			s = shm;
			for (c = 0; c <= 7; c++)
			*s++ = FAILURE[c];
			*s = '\0';
		}

	} 

	closelog ();
}


/*

^006R1!

1.Audio - usb-hub
2.Barcode - uart
3.Camera - usb-hub/direct
4.Fingerprint - usb-hub/direct
5.GPS - usb
6.HUB - usb
7.RFID - I2C/uart

*/

