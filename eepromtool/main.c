#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signal.h>
#include <time.h>

void help_usage(void)
{
    printf("Usage: eepromtool [Option: 1] [Option: 2] [Sector] [Data]\n"
           "[Option: 1]\n"
           "    -w EEPROM Write\n"
           "    -r EEPROM Read\n"
           "    -e EEPROM Erase\n"
           "[Option: 2]\n"
           "    -s [Sector 0 to 255]\n"
           "[Data]\n"
           "    Data to write in EEPROM [Maximum 16 bytes]\n"
           "Write Example: eepromtool -w -s 0 \"Hello World\"\n"
           "Read Example: eepromtool -r -s 0\n"
           "Erase Example: eepromtool -e\n"
           );
}

void handle_alarm( int sig )
{
    printf("Unknown Error\n");
    exit(0);
}

int main (int argc, char *argv[]) {
    int seek_offset;
    char data[16];

    signal( SIGALRM, handle_alarm );

    if(argc > 1)
    {
        if((strcmp(argv[1],"-w")==0) && (argc == 5) ) {
            alarm(2);
            //            if(strcmp(argv[5]),);
            if(strlen(argv[4]) <= 16)
            {
                seek_offset=atoi(argv[3]);
                if(seek_offset<256)
                {
                    FILE *f;
                    if( access( "/sys/bus/i2c/devices/0-0050/eeprom", F_OK ) != -1 )
                    {
                        seek_offset=seek_offset*16;
                        f = fopen ("/sys/bus/i2c/devices/0-0050/eeprom", "w");
                        if(f>=0)
                        {
                            fseek (f, seek_offset, SEEK_SET);
                            fwrite(argv[4],16,1,f);
                            fclose(f);
                            alarm(0);
                        }
                        else
                        {
                            printf("Error While Opening EEPROM\n");
                        }
                    }
                    else
                    {
                        printf("EEPROM Doesn't Exist\n");
                    }
                    printf("Sector %d write successful..!\n",seek_offset/16);
                }
                else
                {
                    printf("Invaild Sector...! Limit= 0 to 255\n"
                           "Help: eepromtool -h\n");
                }
            }
            else {
                printf("Data size exceeded 16 bytes\n");
            }
        }
        else if(strcmp(argv[1],"-r")==0 && argc==4) {
            alarm(2);
            seek_offset=atoi(argv[3]);
            FILE *f;
            if( access( "/sys/bus/i2c/devices/0-0050/eeprom", F_OK ) != -1 )
            {
                if(seek_offset<256) {
                    seek_offset=seek_offset*16;
                    f = fopen ("/sys/bus/i2c/devices/0-0050/eeprom", "r");
                    if(f>=0)
                    {
                        fseek (f, seek_offset, SEEK_SET);
                        fread(data,16,1,f);
                        fclose (f);
                        alarm(0);
                    }
                    else
                    {
                        printf("Error While Openinf EEPROM\n");
                    }
                    printf("%s\n",data);
                }
                else
                {
                    printf("Invaild Sector...! Limit= 0 to 255\n"
                           "Help: eeprom -h\n");
                }
            }
            else
            {
                printf("EEPROM Doesn't Exist\n");
            }
        }
        else if((strcmp(argv[1],"-e")==0) && argc == 2) {
            char null_data[4096];
            memset(null_data,'\0',sizeof(null_data));
            FILE * f = fopen ("/sys/bus/i2c/devices/0-0050/eeprom", "w");
            fwrite(null_data,1,sizeof(null_data),f);
            fclose(f);
            printf("Erasing EEPROM Completed...!\n");
        }
        else if((strcmp(argv[1],"-h")==0) || (strcmp(argv[1],"--help")==0) ){
            help_usage();
        }
        else
        {
            printf("Invaild Arguments...!\n\n");
            help_usage();
        }
    }
    else{
        printf("Invaild Arguments...!\n\n");
        help_usage();
    }
    return 0;
}
