#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define READ	'R'
#define WRITE	'W'
#define DELETE  'D'
#define INSERT  'I'
#define EXIT	'E'
#define ENTER   '\0'

#define BUFFER_SIZE 1024
int readDevice(){
    printf("\nData read from the device:\n");
    system("cat /dev/simple_char_device");
    getInput();
    return 0;
}
int writeDevice(){
    printf("\nEnter data you want to write to the device:\n");
    char buffer[BUFFER_SIZE];
    char data[BUFFER_SIZE];
    if (fgets(data, BUFFER_SIZE, stdin) != NULL){
        char * strippedData;
        if ((strippedData=strchr(data, '\n')) != NULL){
            *strippedData = '\0';
        }
        char cmd[BUFFER_SIZE + 40]= {0x0};
        sprintf(cmd,"\necho \"%s\" >> /dev/simple_char_device", data);
        system(cmd);
    }
    getInput();
    return 0;
}
int DeleteDevice(){
    system("sudo rmmod simple_char_driver");
    getInput();
    return 0;
}
int InsertDevice(){
    system("sudo insmod simple_char_driver.ko");
    getInput();
    return 0;
}
getInput(){
    char inputBuffer[BUFFER_SIZE];
    printf("\n +-------------------------------------------------+");
    printf("\n |-------Simple Char Device to Read,Write,Exit-----|");
    printf("\n +-------------------------------------------------+");
    printf("\n Please enter a command (I,R,W,D,E): ");
    if (fgets(inputBuffer, BUFFER_SIZE, stdin) != NULL){
        /*		if (inputBuffer[0] == OPEN){
                        openDevice();
                }*/
        if (inputBuffer[0] == READ){
            readDevice();
        }
        else if (inputBuffer[0] == WRITE){
            writeDevice();
        }
        else if (inputBuffer[0] == DELETE){
            DeleteDevice();
        }
        else if (inputBuffer[0] == INSERT){
            InsertDevice();
        }
        else if (inputBuffer[0] == EXIT){
        }
        else{
            printf("\nYou have given incorrect arrguments");
            getInput();
        }
    }
}
void main(){
    getInput();
    // return 0;
}
