/*
 * test-sub.c
 * Part of the mosquito-test demonstration application
 * Publishes a fixed number of simple messages to a topic
 * Copyright (c)2016 Kevin Boone. Distributed under the terms of the
 *  GPL v3.0
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <mosquitto.h>
//#include <json/json.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

// Server connection parameters
#define MQTT_HOSTNAME "106.51.48.231"
#define MQTT_PORT 1883
#define MQTT_USERNAME "clanmqtt"
#define MQTT_PASSWORD "clan123"
//#define MQTT_TOPIC "test"

/*
 * Start here
 */

const char s[2] = "~",sbuf[512];
char *token, *r_machine_id, *cmd, machine_id[10], topic[128], *execmd;

int count=0,internet_connection=0,i;
FILE *mfile;

#define PACKETSIZE  64
struct packet
{
    struct icmphdr hdr;
    char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

int pid=-1;
struct protoent *proto=NULL;
int cnt=1;

unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

/*
 * my_message_callback.
 * Called whenever a new message arrives
 */
void my_message_callback(struct mosquitto *mosq, void *obj,
                         const struct mosquitto_message *message)
{
    FILE *fp;
    char machine_id[10];
    fp = fopen("/usr/share/status/EEPROM-data","r");
    fscanf(fp,"%s",machine_id);
    fclose(fp);

    //    printf("Machine ID: %s\n",machine_id);

    // Note: nothing in the Mosquitto docs or examples suggests that we
    //  must free this message structure after processing it.
    //  printf ("%s\n", (char *)message->payload);

    /* get the first token */
    token = strtok((char *)message->payload, s);

    /* walk through other tokens */
    while( token != NULL ) {
        //        printf( "%s\n", token );
        count++;
        switch(count)
        {
        case 1:
            r_machine_id=token;
            break;
        case 2:
            cmd=token;
            break;
        case 3:
            execmd=token;
        }
        token = strtok(NULL, s);
    }
    //     printf("Remote Machine: %s\n",r_machine_id);
    //     printf("Command Received: %s\n",cmd);
    if(strcmp(machine_id,r_machine_id)==0)
    {
        printf("Machine Id Matched\n");
        if(strcmp(cmd,"verlog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);
        }
        if(strcmp(cmd,"kerlog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);
        }
        if(strcmp(cmd,"applog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);
        }
        if(strcmp(cmd,"dblog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);
        }
        if(strcmp(cmd,"permlog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);

        }
        if(strcmp(cmd,"proclog")==0)
        {
            printf("Received Request\n");
            device_republish(cmd);
        }
        if(strcmp(cmd,"screenshot")==0)
        {
            printf("Received Request\n");
            system("sh /opt/daemon_files/screenshot.sh &");
        }
        if(strcmp(cmd,"execcommand")==0)
        {
            printf("Received Request\n");
            system(execmd);
            printf("Command Executed: %s\n",execmd);
        }
    }
    else
    {
        printf("Machine Id not matched\n");
    }
    count=0;
}

static int run = 1;
void handle_signal(int s)
{
    run = 0;
}

int check_net(void){
    int status;
    if( access( "/opt/daemon_files/ping_status", F_OK ) != -1 ) {
        FILE *fp;
        fp = fopen("/opt/daemon_files/ping_status","r");
        char c = fgetc(fp);
        printf("GPRS Character: %c\n",c);
        fclose(fp);
        if(c==0x47) {
            status=0;
        } else {
            status=-1;
        }
    } else {
        status=-1;
    }
    printf("Status: %d\n", status);
    return status;
}

void m_sub_thread_fun(void *vargp)
{
    struct mosquitto *mosq = NULL;
    int rc = 0;

    while(1){
        printf("Thread loop\n");
        // Initialize the Mosquitto library
        if(check_net()==0)
        {
            mosquitto_lib_init();
            // Create a new Mosquito runtime instance with a random client ID,
            //  and no application-specific callback data.
            mosq = mosquitto_new (NULL, true, NULL);
            if (!mosq)
            {
                fprintf (stderr, "Can't init Mosquitto library\n");
            }
            printf("Thread - 1\n");
            // Set up username and password
            mosquitto_username_pw_set (mosq, MQTT_USERNAME, MQTT_PASSWORD);
            // Establish a connection to the MQTT server. Do not use a keep-alive ping
            int ret = mosquitto_connect (mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
            if (ret)
            {
                fprintf (stderr, "Can't connect to Mosquitto server\n");
            }
            printf("Thread - 2\n");
            // Subscribe to the specified topic. Multiple topics can be
            //  subscribed, but only one is used in this simple example.
            //  Note that we don't specify what to do with the received
            //  messages at this point
            ret = mosquitto_subscribe(mosq, NULL, "servercommand", 0);
            if (ret)
            {
                fprintf (stderr, "Can't publish to Mosquitto server\n");
            }

            // Specify the function to call when a new message is received
            mosquitto_message_callback_set (mosq, my_message_callback);
            printf("Thread - 3\n");
            // Wait for new messages
            while(1){
                printf("Thread - 4\n");
                rc = mosquitto_loop(mosq, 1, 1);
                if(check_net()==0){
                    sleep(1);
                    break;
                }
                sleep(1);
            }
            // Tody up. In this simple example, this point is never reached. We can
            //  force the mosquitto_loop_forever call to exit by disconnecting
            //  the session in the message-handling callback if required.
            mosquitto_destroy (mosq);
            mosquitto_lib_cleanup();
        }
        sleep(2);
        printf("Thread working\n");
    }
}

void device_republish(char *command)
{
    struct mosquitto *mosq_sub = NULL;

    char rpayload[20480];

    mosquitto_lib_init();

    // Create a new Mosquito runtime instance with a random client ID,
    //  and no application-specific callback data.
    mosq_sub = mosquitto_new (NULL, true, NULL);
    if (!mosq_sub)
    {
        fprintf (stderr, "Can't initialize Mosquitto library\n");
    }

    mosquitto_username_pw_set (mosq_sub, MQTT_USERNAME, MQTT_PASSWORD);

    // Establish a connection to the MQTT server. Do not use a keep-alive ping
    int ret = mosquitto_connect (mosq_sub, MQTT_HOSTNAME, MQTT_PORT, 0);
    if (ret)
    {
        fprintf (stderr, "Can't connect to Mosquitto server\n");
    }

    if(strcmp(command,"proclog")==0)
    {
        printf("Preparing Proc Log\n");
        system("ps | tail -n 20 > /usr/share/status/debug/proc.log; echo --------------------------------------------------- >> /usr/share/status/debug/proc.log; date >> /usr/share/status/debug/proc.log;echo --------------------------------------------------- >> /usr/share/status/debug/proc.log");
        usleep(700000);

        mfile=fopen("/usr/share/status/debug/proc.log", "r");
        if(mfile==NULL)
        {
            printf("Error in opening file..!!");
        }
        while(fgets(sbuf, 20480, mfile)!=NULL)
        {
            strcat(rpayload,sbuf);
        }
        fclose(mfile);

        strcat(topic,machine_id);
        strcat(topic,"/proc");

        ret = mosquitto_publish (mosq_sub, NULL, topic, strlen(rpayload), rpayload, 0, false);
        if (ret)
        {
            fprintf (stderr, "Can't publish to Mosquitto server\n");
        }
        memset(topic,'\0',sizeof(topic));
        memset(rpayload,'\0',sizeof(rpayload));
    }

    if(strcmp(command,"kerlog")==0)
    {
        printf("Preparing Kernel Log\n");
        system("dmesg | tail -n 100 > /usr/share/status/debug/kernel.log; echo --------------------------------------------------- >> /usr/share/status/debug/kernel.log; date >> /usr/share/status/debug/kernel.log;echo --------------------------------------------------- >> /usr/share/status/debug/kernel.log");
        usleep(700000);

        mfile=fopen("/usr/share/status/debug/kernel.log", "r");
        if(mfile==NULL)
        {
            printf("Error in opening file..!!");
        }
        while(fgets(sbuf, 20480, mfile)!=NULL)
        {
            strcat(rpayload,sbuf);
        }
        fclose(mfile);

        strcat(topic,machine_id);
        strcat(topic,"/dmesg");

        ret = mosquitto_publish (mosq_sub, NULL, topic, strlen(rpayload), rpayload, 0, false);
        if (ret)
        {
            fprintf (stderr, "Can't publish to Mosquitto server\n");
        }
        memset(topic,'\0',sizeof(topic));
        memset(rpayload,'\0',sizeof(rpayload));
    }

    if(strcmp(command,"verlog")==0)
    {
        printf("Preparing Version Log\n");
        usleep(700000);
        mfile=fopen("/usr/share/status/OS-Version", "r");
        if(mfile==NULL)
        {
            printf("Error in opening file..!!");
        }
        while(fgets(sbuf, 20480, mfile)!=NULL)
        {
            strcat(rpayload,sbuf);
        }
        fclose(mfile);

        strcat(topic,machine_id);
        strcat(topic,"/ver");

        ret = mosquitto_publish (mosq_sub, NULL, topic, strlen(rpayload), rpayload, 0, false);
        if (ret)
        {
            fprintf (stderr, "Can't publish to Mosquitto server\n");
        }
        memset(topic,'\0',sizeof(topic));
        memset(rpayload,'\0',sizeof(rpayload));
    }

    if(strcmp(command,"applog")==0)
    {
        printf("Preparing Application Log\n");
        system("cat /home/root/LOG | tail -n 100 > /usr/share/status/debug/app.log; echo --------------------------------------------------- >> /usr/share/status/debug/app.log; date >> /usr/share/status/debug/app.log;echo --------------------------------------------------- >> /usr/share/status/debug/app.log");
        usleep(700000);

        mfile=fopen("/usr/share/status/debug/app.log", "r");
        if(mfile==NULL)
        {
            printf("Error in opening file..!!");
        }
        while(fgets(sbuf, 20480, mfile)!=NULL)
        {
            strcat(rpayload,sbuf);
        }
        fclose(mfile);

        strcat(topic,machine_id);
        strcat(topic,"/applog");

        ret = mosquitto_publish (mosq_sub, NULL, topic, strlen(rpayload), rpayload, 0, false);
        if (ret)
        {
            fprintf (stderr, "Can't publish to Mosquitto server\n");
        }
        memset(topic,'\0',sizeof(topic));
        memset(rpayload,'\0',sizeof(rpayload));
    }
    mosquitto_disconnect (mosq_sub);
    mosquitto_destroy (mosq_sub);
    mosquitto_lib_cleanup();

}

char *my_itoa(int num, char *str)
{
    if(str == NULL)
    {
        return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}

int main (int argc, char *argv[])
{
    int connection_status=0;
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

    pthread_t m_sub;
    pthread_create(&m_sub,NULL,m_sub_thread_fun,NULL);
    // pthread_join(m_sub, NULL);

    time_t rawtime;
    struct tm * timeinfo;

    char mpayload[20480], s[512], ping_stat, charger_status[5], pp_status[5], pp_length[5], device_time[32], network_mode[5], bat_status[5], mem_status[1024], cpu_present[5], gps_status[32], time_status[32];
    char mpayloadTemp[20480]; // used to concatenate HW health counters
    int battery_input, percentage;

    printf("Start Time Stamp: %d\n", (int)time(NULL));
    while(1){
        printf("Internet Connection: %d\n", internet_connection);
        if(check_net()==0){
            struct mosquitto *mosq = NULL;

            mfile=fopen("/usr/share/status/EEPROM-data","r");
            fscanf(mfile,"%s",machine_id);
            fclose(mfile);

            // Initialize the Mosquitto library
            mosquitto_lib_init();
            printf("Mosquitto Lib Init\n");

            // Create a new Mosquito runtime instance with a random client ID,
            //  and no application-specific callback data.
            mosq = mosquitto_new (NULL, true, NULL);
            if (!mosq)
            {
                fprintf (stderr, "Can't initialize Mosquitto library\n");
                connection_status=0;
            }
            else
            {
                connection_status=1;
            }

            mosquitto_username_pw_set (mosq, MQTT_USERNAME, MQTT_PASSWORD);

            // Establish a connection to the MQTT server. Do not use a keep-alive ping
            int ret = mosquitto_connect (mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
            if (ret)
            {
                fprintf (stderr, "Can't connect to Mosquitto server\n");
                connection_status=0;
            }
            else
            {
                connection_status=1;
            }
            if(connection_status==1)
            {
                printf("Sending Data\n");

                //<**************Machine ID************>
                strcat(mpayload,"{\"deviceid\":\"");
                strcat(mpayload,machine_id);
                strcat(mpayload,"\",");
                //<**************Machine ID************>

                //<**************Time************>
                //                time ( &rawtime );
                //                timeinfo = localtime ( &rawtime );
                my_itoa((int)time(NULL),time_status);
                strcat(mpayload,"\"timestamp\":\"");
                strcat(mpayload,time_status);
                strcat(mpayload,"\",");
                //<**************Time************>

                //<**************Charger Status************>
                //            printf("Charger\n");
                strcat(mpayload,"\"chr_stat\":\"");
                system("cat /sys/class/gpio/gpio110/value 1> /usr/share/status/debug/chr.log");
                mfile = fopen("/usr/share/status/debug/chr.log","r");
                if(mfile)
                {
                    fscanf(mfile,"%s",charger_status);
                    fclose(mfile);
                }
                strcat(mpayload,charger_status);
                strcat(mpayload,"\",");
                //<**************Charger Status************>

                //<**************Paper Presence Status************>
                //            printf("PP Sense\n");
                strcat(mpayload,"\"paper_presence\":\"");
                mfile = fopen("/usr/share/status/PRINTER_status","r");
                if(mfile){
                    fscanf(mfile,"%s",pp_status);
                    fclose(mfile);
                }
                strcat(mpayload,pp_status);
                strcat(mpayload,"\",");
                //<**************Paper Presence Status************>

                //<**************Paper Length Status************>
                //            printf("Paper Length\n");
                strcat(mpayload,"\"printed_length\":\"");
                mfile = fopen("/usr/share/status/debug/printLength.log","r");
                if(mfile)
                {
                    fscanf(mfile,"%s",pp_length);
                    fclose(mfile);
                }
                strcat(mpayload,pp_length);
                strcat(mpayload,"\",");
                //<**************Paper Length Status************>

                //<***************Device ON Status***************>
                strcat(mpayload,"\"device_on_time\":\"");
                mfile = fopen("/usr/share/status/debug/deviceOnStatus.log","r");
                if(mfile)
                {
                    fscanf(mfile,"%s", device_time);
                    fclose(mfile);
                }
                strcat(mpayload,device_time);
                strcat(mpayload,"\",");
                //<***************Device ON Status***************>

                //<**************Network Status************>
                //            printf("Network \n");
                strcat(mpayload,"\"network_mode\":\"");
                mfile = fopen("/opt/daemon_files/ping_status","r");
                if(mfile)
                {
                    fscanf(mfile,"%s", network_mode);
                    fclose(mfile);
                }
                strcat(mpayload,network_mode);
                strcat(mpayload,"\",");
                //<**************Network Status************>

                //<**************Battery Status************>
                //            printf("Battery\n");
                strcat(mpayload,"\"batt_precent\":\"");
                system("cat /sys/class/power_supply/NUC970Bat/present > /usr/share/status/debug/bat.log");
                mfile = fopen("/usr/share/status/debug/bat.log","r");
                if(mfile)
                {
                    fscanf(mfile,"%s",bat_status);
                    fclose(mfile);
                }
                //            printf("Battery: %s\n", bat_status);
                battery_input = atoi(bat_status);
                percentage = ((battery_input-75)*100)/(98-75);
                //            printf("Battery Per: %d\n",percentage);
                my_itoa(percentage,bat_status);
                strcat(mpayload,bat_status);
                strcat(mpayload,"\",");
                //<**************Battery Status************>

                //<**************Memory Status************>
                //                printf("Memory\n");
                //                strcat(mpayload,"\"mem_stat\":\"");
                //                system("sh /opt/mem_status");
                //                mfile=fopen("/usr/share/status/mem_state", "r");
                //                if(mfile==NULL)
                //                {
                //                    printf("Error in opening file..!!");
                //                }
                //                while(fgets(s, 20480, mfile)!=NULL)
                //                {
                //                    strcat(mem_status,s);
                //                }
                //                fclose(mfile);
                //                strcat(mpayload,mem_status);
                //                strcat(mpayload,"\",");
                //<**************Memory Status************>

                //                //<**************Cpu Status************>
                //                //            printf("CPU\n");
                //                strcat(mpayload,"\"cpu_percent\":\"");
                //                system("cpu=`top -n 1 | grep \"[i]dle\" | awk '{print $2}'`; echo ${cpu//%} 1> /usr/share/status/debug/cpu.log");
                //                mfile = fopen("/usr/share/status/debug/cpu.log","r");
                //                if(mfile)
                //                {
                //                    fscanf(mfile,"%s",cpu_present);
                //                    fclose(mfile);
                //                }
                //                strcat(mpayload,cpu_present);
                //                strcat(mpayload,"\",");
                //                //<**************Cpu Status************>

                //<**************GPS Status************>
                //            printf("GPS\n");
                strcat(mpayload,"\"gps_loc\":\"");
                if(mfile=fopen("/usr/share/status/GPS_DATA", "r"))
                {
                    if(mfile==NULL)
                    {
                        printf("Error in opening file..!!");
                    }
                    while(fgets(s, 20480, mfile)!=NULL)
                    {
                        for(i=0;i<sizeof(s);i++){
                            if(s[i]=='\n'){
                                s[i]='\0';
                            }
                        }
                        strcat(gps_status,s);
                    }
                    fclose(mfile);
                }
                else{
                    strcat(gps_status,"Not Fixed");
                }
                strcat(mpayload,gps_status);
                strcat(mpayload,"\"}");
                //<**************GPS Status************>
                strcat(topic,machine_id);
                strcat(topic,"/data");

                printf("Topic: %s\n", topic);
                printf("Json Data: %s\n", mpayload);

                ret = mosquitto_publish (mosq, NULL, topic, strlen(mpayload), mpayload, 0, false);
                if (ret)
                {
                    fprintf(stderr, "Can't publish to Mosquitto server\n");
                }
                memset(mpayload,'\0',sizeof(mpayload));
                memset(gps_status, '\0', sizeof(gps_status));
                memset(cpu_present, '\0', sizeof(cpu_present));
                memset(mem_status, '\0', sizeof(mem_status));
                memset(cpu_present, '\0', sizeof(cpu_present));
                memset(charger_status,'\0', sizeof(charger_status));
                memset(pp_status,'\0', sizeof(pp_status));
                memset(pp_length,'\0', sizeof(pp_length));
                memset(topic,'\0',sizeof(topic));
                sleep(60);
            }
            // Tidy up
            mosquitto_disconnect (mosq);
            mosquitto_destroy (mosq);
            mosquitto_lib_cleanup();
        }
        else
        {
            printf("Unable to ping internet\n");
            sleep(2);
        }
    }
    return 0;
}
