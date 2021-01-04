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
#include <dirent.h>

#define SHMSZ     27
#define ERROR(fmt, ...) do { printf(fmt, __VA_ARGS__); return -1; } while(0)

#define audio_power 167
#define camera_reg1 168 
#define camera_reg2 111
#define future_power 163
#define gps_power 174
#define gps_fix 173
#define rfid_power 35
#define rfid_io_enable 109
#define bar_power 196
#define bar_trig 197
#define usb_hub_enable 143
#define fingerprint_power 166
#define five_volt 43
#define sam_select 107

struct ifreq ifr;
struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;

char WAITING[8] = "WAITING";
char SUCCESS[8] = "SUCCESS";
char FAILURE[8] = "FAILURE";

char status_buff[100],gprs_power[2],wifi_power[2],gpssat[5];
int v5=0,usb_hub=0,fp=0,magnetic=0,rfid=0,sam=0,smart=0,camera=0,bar=0,audio=0,gps=0,idel=0;
char module_on_off[20];

FILE *fp_remote_modules;
FILE *fgprs_power;
FILE *fwifi_power;
FILE *fp_gps;

pid_t nfcp;

int i=0;

//------------ GPIO ------------------------


void gpio_init(void);
void gpio_process(int pin_no, int direction);

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

void gpio_init()
{
    gpio_process(290,1); //Battery Status 1
    gpio_process(292,1); //Battery Status 2
    gpio_process(42,0);  //3.8v Regulator
    gpio_process(288,0); //GPRS Enable


}

void gpio(int pin_no,char gpiovalue)
{
    char start[100]="/sys/class/gpio/gpio";
    char end[100]="/value";
    char gpio_pin[5];
    char value[5];
    sprintf(gpio_pin, "%d", pin_no);

    strcat(start,gpio_pin);

    strcat(start,end);


    FILE *fgpio;

    fgpio=fopen(start, "w");
    if(fgpio)
    {
        fwrite(&gpiovalue,1,1,fgpio);
        fclose(fgpio);
    }
}
/* 0 = Direction Out */
/* 1 = Direction Out */
void gpio_process(int pin_no, int direction)
{
    char start[100]="/sys/class/gpio/gpio";
    char end[100]="/direction";
    char gpio_pin[5];
    sprintf(gpio_pin, "%d", pin_no);

    strcat(start,gpio_pin);
    strcat(start,end);

    FILE *egpio, *dgpio;
    egpio = fopen("/sys/class/gpio/export","w");
    if(egpio)
    {
        fprintf(egpio,"%d",pin_no);
        fclose(egpio);
    }

    dgpio = fopen(start,"w");
    if(dgpio)
    {
        if(direction==0)
        {
            fprintf(dgpio,"%s","out");
        }
        else if(direction==1)
        {
            fprintf(dgpio,"%s","in");
        }
        fclose(dgpio);
    }
}

int main(int argc, char *argv[]) {

    int c;
    int shmid;
    key_t key;
    char *shm, *s;

    key = 3456;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;

    for (c = 0; c <= 7; c++)
        *s++ = WAITING[c];
    *s = '\0';

    //printf("send\n");
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

    //----------------
    //Main Process
    //---------------

    while(1)
    {
        while (*shm != '^')
            sleep(1);
        //        printf("received.......\n");
        int i=0;
        for (s = shm; *s != '\0'; s++)
        {
            //            printf("%c\n",status_buff[i]);
            status_buff[i]=*s;
            i++;
        }

        // ---------------Success -------------------

        if(status_buff[0]=='^' && status_buff[6]=='!')
        {
            // ---------------- Audio (^001A1!) --------------------

            if(status_buff[2]=='0' && status_buff[3]=='1' && status_buff[4]=='A')
            {

                if(status_buff[5]=='1')
                {
                    //                    if(usb_hub==0)
                    //                    {
                    //                        gpio(usb_hub_enable,'1');
                    //                        usb_hub=1;
                    //                    }
                    if(audio==0)
                    {
                        //gpio(audio_camera_switch,'1');
                        gpio(audio_power,'1');
                        audio=1;
                    }
                }
                else
                {
                    //                    if(usb_hub!=0)
                    //                    {
                    //                        gpio(usb_hub_enable,'0');
                    //                        usb_hub=0;
                    //                    }
                    if(audio!=0)
                    {
                        gpio(audio_power,'0');
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
                        gpio(bar_trig,'1');
                        gpio(bar_power,'1');
                        bar=1;
                    }
                }
                else if(status_buff[5]=='2')  // 2 - Bar code reader trigger Trigger
                {
                    gpio(bar_trig,'0');
                    //					printf("barcode trigger\n");
                }
                else if(status_buff[5]=='3')  // 2 - Bar code reader trigger Trigger
                {
                    gpio(bar_trig,'1');
                    //					printf("barcode trigger\n");
                }
                else
                {
                    if(bar!=0)
                    {
                        //	v5--;
                        //					printf("barcode disable\n");
                        gpio(bar_trig,'1');
                        gpio(bar_power,'0');
                        bar=0;
                    }
                }

            }
            // ---------------- Camera (^003C1!) --------------------

            else if(status_buff[2]=='0' && status_buff[3]=='3' && status_buff[4]=='C')
            {

                if(status_buff[5]=='1')
                {
                    //                    if(usb_hub==0)
                    //                    {
                    //                        gpio(usb_hub_enable,'1');
                    //                        usb_hub=1;
                    //                    }
                    if(camera==0)
                    {
                        gpio(camera_reg1,'1');
                        usleep(500);
                        gpio(camera_reg2,'1');
                        usleep(1000);
                        gpio(future_power,'1');
                        camera=1;
                    }
                }
                else
                {
                    //                    if(usb_hub!=0)
                    //                    {
                    //                        gpio(usb_hub_enable,'0');
                    //                        usb_hub=0;
                    //                    }
                    if(camera!=0)
                    {
                        gpio(camera_reg1,'0');
                        usleep(500);
                        gpio(camera_reg2,'0');
                        usleep(1000);
                        gpio(future_power,'0');
                        camera=0;
                    }
                }

            }
            // ---------------- Fingerprint (^004F1!) --------------------

            else if(status_buff[2]=='0' && status_buff[3]=='4' && status_buff[4]=='F')
            {

                if(status_buff[5]=='1')
                {
                    //                    if(usb_hub==0)
                    //                    {
                    //                        gpio(usb_hub_enable,'1');
                    //                        usb_hub=1;
                    //                    }
                    if(fp==0)
                    {
                        gpio(fingerprint_power,'1');
                        fp=1;
                    }
                }
                else
                {
                    //                    if(usb_hub!=0)
                    //                    {
                    //                        gpio(usb_hub_enable,'0');
                    //                        usb_hub=0;
                    //                    }
                    if(fp!=0)
                    {
                        gpio(fingerprint_power,'0');
                        fp=0;
                    }
                }

            }

            // ---------------- Magnetic card (^005M1!) --------------------
            /*
else if(status_buff[2]=='0' && status_buff[3]=='5' && status_buff[4]=='M')
{

if(status_buff[5]=='1')
{
if(magnetic==0)
{
v5++;
gpio(bar_mag_switch,'0');
gpio(magnetic_power,'1');
magnetic=1;
}
}
else
{
if(magnetic!=0)
{
v5--;
gpio(bar_mag_switch,'0');
gpio(magnetic_power,'0');
magnetic=0;
}
}

}
*/
            // ---------------- RFID (^006R1!) --------------------

            if(status_buff[2]=='0' && status_buff[3]=='6' && status_buff[4]=='R')
            {

                if(status_buff[5]=='1')
                {
                    if(rfid==0)
                    {
                        //system("insmod /opt/daemon_files/pn5xx_i2c.ko");
                        //usleep(1000);
                        //gpio(rfid_power,'1');
                        //usleep(1000);
                        //gpio(rfid_io_enable,'1');
                        rfid=1;
                    }
                }
                else
                {
                    if(rfid!=0)
                    {
                        //system("rmmod /opt/daemon_files/pn5xx_i2c.ko");
                        //usleep(1000);
                        //gpio(rfid_power,'0');
                        //usleep(1000);
                        //gpio(rfid_io_enable,'0');
                        rfid=0;
                    }
                }

            }

            // ---------------- SAM Card (^007S1!) --------------------
            /*
            else if(status_buff[2]=='0' && status_buff[3]=='7' && status_buff[4]=='S')
            {
                if(status_buff[5]=='1')
                {
                    if(sam==0)
                    {
                        v5++;
                        usb_hub++;
                        sam=1;
                    }
                }
                else
                {
                    if(sam!=0)
                    {
                        v5--;
                        usb_hub--;
                        sam=0;
                    }
                }

            }
*/
            // ---------------- SMARD Card (^008S1!) --------------------
/*
            else if(status_buff[2]=='0' && status_buff[3]=='8' && status_buff[4]=='S')
            {

                if(status_buff[5]=='1')
                {
                    if(smart==0)
                    {
                        v5++;
                        usb_hub++;
                        smart=1;
                    }
                }
                else
                {
                    if(smart!=0)
                    {
                        v5--;
                        usb_hub--;
                        smart=0;
                    }
                }

            }
*/
            // ---------------- GPS (^009G1!) --------------------

            else if(status_buff[2]=='0' && status_buff[3]=='9' && status_buff[4]=='G')
            {

                if(status_buff[5]=='1')
                {
                    if(gps==0)
                    {
                        gpio(gps_power,'1');
                        //gpio(GPS_RST,'1');
                        //sleep(1);
                        //gpio(gps_power,'0');
                        //sleep(1);
                        //gpio(GPS_RST,'1');
                        gps=1;
                    }
                }
                else
                {
                    if(gps!=0)
                    {
                        gpio(gps_power,'0');
                        gps=0;
                    }
                }

            }
            /*
// ---------------- IDEL (^015I1!) --------------------
*
else if(status_buff[2]=='1' && status_buff[3]=='5' && status_buff[4]=='I')
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

fwifi_power=fopen("/sys/class/gpio/gpio248/value","r");
fread(wifi_power,1,1,fwifi_power);
fclose(fwifi_power);
*/
            // --------------- 5V_per --------------------------
            /*
if(v5<=0)
{
        if(gprs_power[0]=='1' || wifi_power[0]=='1')
        {
        if(idel==0)
        {
        gpio(five_volt,'1');
        }
        else
        {
        gpio(five_volt,'0');
        }
        }
        else
        {
        gpio(five_volt,'0');
        }
}
else
{
        if(idel==0)
        {
        gpio(five_volt,'1');
        }
        else
        {
        gpio(five_volt,'0');
        }
}

// --------------- usb_hub --------------------------

if(usb_hub<=0)
{
        if(wifi_power[0]=='1')
        {
        gpio(usb_hub_enable,'1');
        }
        else
        {
        gpio(usb_hub_enable,'0');
        }
}
else
{
        gpio(usb_hub_enable,'1');
}
*/
            //            printf("testing-------------\n");
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

1.Audio
2.Barcode
3.BlueTooth
4.Camera
5.External USB
6.Fingerprint
7.GPRS
8.GPS
9.HUB
10.Magnetic Card
11.Printer
12.RFID
13.SAM
14.Smartcard
15.WiFi

*/

