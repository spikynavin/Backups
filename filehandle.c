#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024;

#define OPEN  = 'O';
#define CREAT = 'C';
#define READ  = 'R';
#define WRITE = 'W';
#define EXIT  = 'E';

/*--------------------------------Create File------------------*/
createfile(){
    char fname[20];
    char data[1024];
    char ch;

    printf("Enter the file name: \n");
    scanf("%s", fname);
    fp = fopen(fname, "w");
    if (fp == NULL)
    {
        printf("File does not exists \n");
        return;
    }
    getinput();
}
        openfile(){
            char ch;
            char fname[200];
            fp=fopen(fname,"r");
            if (fp == NULL)
            {
                printf("Cannot open file \n");
                return;}
            getinput();
        }

        readfile(){
            char ch;
            char fname[200];
            fp=fopen(fname,"r");
            getinput();
        }

        writefile(){
            char ch;
            char fname[200];
            char data[1024];
            fd=fopen(fname,"w");
            printf("Enter the data \n");
            scanf("%s", data);
            fprintf(fp,"Data = %s\n", data);
            fclose(fp);
            ch = fgetc(fp);
            while (ch != EOF){
                printf("%c",ch);
                ch = fgetc(fp);
            }
            fclose(fp);
            getinput();
        }

        /* ---------------------- INPUTS----------------------*/
        getinput(){
            char inputBuffer[BUFFER_SIZE];
            printf("\nPlease enter a command (O,C,R,W,E): ");
            if (fgets(inputBuffer, BUFFER_SIZE, stdin) != NULL){
                if (inputBuffer[0] == OPEN){
                    openfile();
                }
                else if (inputBuffer[0] == NEW){
                    createfile();
                }
                else if (inputBuffer[0] == READ){
                    readfile();
                }
                else if (inputBuffer[0] == WRITE){
                    writefile();
                }
                else if (inputBuffer[0] == EXIT){

                }
                else {
                    printf("Enter the correct option");
                    getinput();
                }
            }
        }
                /*--------------------------Main Function------------------------*/
                main(){
                    getinput();
                }



