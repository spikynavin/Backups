#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <stdlib.h>
#include <string.h>

/*-----------------------------------------------Opening & reading the log file----------------------------------*/
int word_search(char*filepath,char*word)
{
    FILE *fp;
    char line[200];

    fp=fopen(filepath,"r");
    if(!fp)
    {
        perror("could not find the file");
        exit(0);
    }
    while ( fgets ( line, 200, fp ) != NULL ) /* read a line */
    {
        if(strstr(line,word))
        {
            fputs ( line, stdout ); /* write the line */
            return 1;
        } else if(strstr(line,"Error")) {
            return -2;
        }
        else
        {
            return -1;
        }
    }
    fclose ( fp );
    return 0;
    /*------------------------------------------Make the Daemon in the background process----------------------------------*/
}

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

    int fd; // file discripter //

    printf("\n +----------------------------------+");
    printf("\n |      Machine ID Handler Tool     |");
    printf("\n +----------------------------------+");

    /*------------------------------- Opening the Serial Port of 751XX -------------------------------*/

    /* Change /dev/ttyUSB0 to the one corresponding to your system */

    fd = open("/dev/ttyGS0",O_RDWR | O_NOCTTY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
    /* O_RDWR   - Read/Write access to serial port       */
    /* O_NOCTTY - No terminal will control the process   */
    /* Open in blocking mode,read will wait              */
    fcntl(fd, F_SETFL, O_NONBLOCK);



    if(fd == -1)						/* Error Checking */
        printf("\n  Error! in Opening ttyGS0  ");
    else
        printf("\n  Ports Opened Successfully ");


    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    /* Setting the Baud rate */
    cfsetispeed(&SerialPortSettings,B115200); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 9600                       */

    /* 8N1 Mode */
    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    /* Setting Time outs */
    SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
    SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none\n");

    /*------------------------------- Read data from serial port -----------------------------*/

    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */
    char read_buffer[1024];   /* Buffer to store the data received              */
    char write_buffer[1024];  /* Buffer to write the data to port  */
    int  bytes_read = 0;    /* Number of bytes read by the read() system call */
    /*	usleep(100);  */
    printf("\nWaiting For Request..!\n");
    while(1)
    {
        usleep(50000);
        bytes_read = read(fd,&read_buffer,1024); /*   Read the data   */

        if(bytes_read > 0)
        {
            printf("%s\n",read_buffer);
            if(read_buffer[0]=='~' && read_buffer[1]=='C' && read_buffer[2]=='!')
            {
                write(fd,"~OK!",4);
            }else if(read_buffer[0]=='~' && read_buffer[1]=='M' && read_buffer[2]=='I' && read_buffer[3]=='~' && read_buffer[5]=='~' && read_buffer[6]=='!'){
                /*-------------------------------------Verifying Machine ID----------------------------------------*/
                printf("Verifing Machine ID\n");
                char str[1024] = "~OK~";
                char str1[16];
                if(read_buffer[4]=='V'){
                    system("eepromtool -r -s 0 > /tmp/eeprom.log");
                    int ret = word_search("/tmp/eeprom.log","751");
                    if(ret == 1) {
                        FILE *fp = fopen("/tmp/eeprom.log","r");
                        fscanf(fp,"%s",str1);
                        fclose(fp);

                        strcat(str,str1);
                        strcat(str,"~!");

                        printf("Word Found: %s \n", str);
                        write(fd,str,sizeof(str));
                    }else if(ret == -2 ){
                        printf("Not found\n");
                        write(fd,"~ERROR~3~!",10);
                    } else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~1~!",10);
                    }
                }
                else if(read_buffer[4]=='D') {
                    /*-------------------------------------Deleting Machine ID----------------------------------------*/
                    printf("Deleting EEPROM\n");
                    system("eepromtool -w -s 0 \"\" > /tmp/eeprom.log");
                    int ret = word_search("/tmp/eeprom.log","successful..!");
                    if(ret == 1) {
                        printf("Word found\n");
                        write(fd,"~OK!",4);
                    } else if(ret == -2){
                        printf("Not found\n");
                        write(fd,"~ERROR~4~!",10);
                    } else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~2~!",10);
                    }
                }
                system("rm /tmp/eeprom.log");
            } else if(read_buffer[0]=='~' && read_buffer[1]=='M' && read_buffer[2]=='N' && read_buffer[3]=='~' && read_buffer[5]=='~' && read_buffer[6]=='!'){
                /*-------------------------------------Verifying Machine Model NUmber----------------------------------------*/
                printf("Verifing Model Number\n");
                char str[1024] = "~OK~";
                char str1[16];
                if(read_buffer[4]=='V'){
                    system("eepromtool -r -s 1 > /tmp/eeprom.log");
                    int ret = word_search("/tmp/eeprom.log","HHC");
                    if(ret == 1) {
                        FILE *fp = fopen("/tmp/eeprom.log","r");
                        fscanf(fp,"%s",str1);
                        fclose(fp);

                        strcat(str,str1);
                        strcat(str,"~!");

                        printf("Word Found: %s \n", str);
                        write(fd,str,sizeof(str));
                    }else if(ret == -2 ){
                        printf("Not found\n");
                        write(fd,"~ERROR~5~!",10);
                    } else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~3~!",10);
                    }
                }
                else if(read_buffer[4]=='D') {
                   /*-------------------------------------Deleting Machine Model Number----------------------------------------*/
                    printf("Deleting EEPROM\n");
                    system("eepromtool -w -s 1 \"\" > /tmp/eeprom.log");
                    int ret = word_search("/tmp/eeprom.log","successful..!");
                    if(ret == 1) {
                        printf("Word found\n");
                        write(fd,"~OK!",4);
                    }else if(ret == -2 ){
                        printf("Not found\n");
                        write(fd,"~ERROR~6~!",10);
                    } else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~4~!",10);
                    }
                }
                system("rm /tmp/eeprom.log");
            } else if(read_buffer[0]=='~' && read_buffer[1]=='M' && read_buffer[2]=='I' && read_buffer[3]=='~' && read_buffer[bytes_read-4]=='~' && read_buffer[bytes_read-3]=='!')
            {
                /*-------------------------------------Writing Machine ID----------------------------------------*/
                printf("Writing Machine ID\n");
                //     char str3[20]="~MI~7510XX0000~!";
                const char s[2]="~";
                char *token, count=0;
                count ++;
                token=strtok(read_buffer, s);
                while(token !=NULL)
                {
                    count++;
                    if(count==3){
                        printf("%s\n",token);
                        break;
                    }
                    token =strtok(NULL, s);
                }

                printf("Debug:%s\n",token);

                printf("%d\n",strlen(token));

                if(strlen(token) <= 9)
                {
                    printf("Writing EEPROM\n");
                    char str1[40]="eepromtool -w -s 0 \"";
                    char str2[]="\" > /tmp/eeprom.log";
                    strcat(str1,token);
                    strcat(str1,str2);
                    system(str1);
                    printf("%s\n",str1);
                }

                int ret = word_search("/tmp/eeprom.log","successful..!");
                if(ret == 1) {
                    printf("Word found\n");
                    write(fd,"~OK!",4);
                }else {
                    int ret = word_search("/tmp/eeprom.log","Unknown Error");
                    if(ret == 1) {
                        write(fd,"~ERROR~1~!",11);
                    }else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~6~!",10);
                    }
                }
                system("rm /tmp/eeprom.log");
            }
            else if(read_buffer[0]=='~' && read_buffer[1]=='M' && read_buffer[2]=='N' && read_buffer[3]=='~' && read_buffer[bytes_read-4]=='~' && read_buffer[bytes_read-3]=='!')
            {
                /*-------------------------------------Writing Machine Serial Number----------------------------------------*/
                printf("Writing Model Number\n");
                //     char str3[20]="~MI~7510XX0000~!";
                const char s[2]="~";
                char *token, count=0;
                count ++;
                token=strtok(read_buffer, s);
                while(token !=NULL)
                {
                    count++;
                    if(count==3){
                        printf("%s\n",token);
                        break;
                    }
                    token =strtok(NULL, s);
                }
                printf("Debug:%s\n",token);
                printf("%d\n",strlen(token));
                if(strlen(token) <= 7)
                {
                    printf("Writing EEPROM\n");
                    char str1[40]="eepromtool -w -s 1 \"";
                    char str2[]="\" > /tmp/eeprom.log";
                    strcat(str1,token);
                    strcat(str1,str2);
                    system(str1);
                    printf("%s\n",str1);
                }
                int ret = word_search("/tmp/eeprom.log","successful..!");
                if(ret == 1) {
                    printf("Word found\n");
                    write(fd,"~OK!",4);
                }else {
                    int ret = word_search("/tmp/eeprom.log","Unknown Error");
                    if(ret == 1) {
                        write(fd,"~ERROR~2~!",11);
                    }else if(ret == -1){
                        printf("Empty File\n");
                        write(fd,"~EMPTY~6~!",10);
                    }
                }
                system("rm /tmp/eeprom.log");
            }
            else{
                write(fd,"~ERROR~!",8);
            }
        }
        memset(read_buffer,0,sizeof(read_buffer));
    }
    return 0;
}
