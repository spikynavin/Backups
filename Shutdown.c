#include <stdio.h>
#include <stdlib.h>
#define POWEROFF  'P'
#define RESTART   'R'
#define SLEEP     'S'
#define CANCEL    'C'

#define BUFFER_SIZE 200
int i,j;
int Poweroff(){
    printf("\n\nIm going to off...BYE..!!!\n");
    time_count();
    system("shutdown -h now");
    getInput();
    return 0;
}

int Restart(){
    printf("\n\nI will be right back in a minute...!!!\n");
    time_count();
    system("shutdown -r now ");
    getInput();
    return 0;
}

int Sleep(){
    printf("\n\nJust wakeme up using Keyclick...!!!\n");
    time_count();
    system("systemctl suspend ");
    //  printf("Im not going to Sleep\n");
    getInput();
    return 0;
}
time_count(){
    for (i=0;i<10;i++)
    {
        printf("\033[A\33[2KT\r Count:%d\n",i);
        sleep(1);
    }
    return 0;
}
count_minutes(){
    for (j=0;j<1000;j++){
        printf("Ok\n");
    }
    return 0;
}

getInput(){
    char inputBuffer[BUFFER_SIZE];
    printf("\n +-------------------------------------------------+");
    printf("\n |                                                 |");
    printf("\n |             System Power Handler Tool           |");
    printf("\n |                                                 |");
    printf("\n +-------------------------------------------------+");
    printf("\n\n\n Please enter a command (P,R,S,C): ");
    if (fgets(inputBuffer, BUFFER_SIZE, stdin) != NULL){
        if (inputBuffer[0] == POWEROFF){
            Poweroff();
        }
        if (inputBuffer[0] == RESTART){
            Restart();
        }
        else if (inputBuffer[0] == SLEEP){
            Sleep();
        }
        else if (inputBuffer[0] == CANCEL){
        }
        else{
            printf("\n\nYou have given incorrect arrguments");
            getInput();
        }
    }
}
void main(){
    getInput();
    // return 0;
}



