#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE 1024

#define OPEN   'C'
#define WRITE  'W'
#define EXIT   'E'
#define READ   'R'
#define LIST   'L'


FILE*fp;
char ch;
char fname[20];
char data[1024];

void createfile(){
    printf("Enter the file name: \n");
    gets(fname);
    fp = fopen(fname, "w");
    if (fp == NULL)
    {
        printf("File does not exists \n");
        exit(0);
    }
    printf("File create Successfully\n");
    printf("File name: %s\n",fname);
    getinput();
  //  return;
}

void writefile(){
    printf("Enter the data \n");
    gets(data);
    fprintf(fp,"Data = %s\n", data);
    if(data==NULL){
    printf("No data is written");
    }
    fclose(fp);
    getinput();
  //  return;
} 

void readfile(){
    printf("Enter file name to read: ");
    gets(fname);
    fp=fopen(fname,"r");
    if(fp==NULL){
    printf("File not exits");
    }
    ch = fgetc(fp);
    while (ch != EOF)
    {
    printf ("%c", ch);
    ch = fgetc(fp);
    }
    fclose(fp);
    getinput();
  //  return;
}

void listfile(){
DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d)
    {
    while ((dir = readdir(d)) != NULL)
    {
    printf("%s\n", dir->d_name);
    }
    closedir(d);
    }
    getinput();
  //  return;
}

int getinput(){
    char inputBuffer[BUFFER_SIZE];
    printf("\nEnter a command (C,W,R,L,E): ");
    if (fgets(inputBuffer, BUFFER_SIZE, stdin) != NULL){
    if(inputBuffer[0] == OPEN){
    createfile();
    }
    else if (inputBuffer[0] == LIST){
    listfile();
    }
    else if (inputBuffer[0] == WRITE){
    writefile();
    }
    else if (inputBuffer[0] == READ){
    readfile();
    }
    else if (inputBuffer[0] == EXIT){
    }
    else {
    printf("Give correct option");
    getinput();}
}
}
int main(){
void listfile();
void createfile();
void writefile();
void readfile();
getinput();
}
