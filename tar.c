#include<stdio.h>
#include<stdlib.h>

#define buffer 1024

struct tarfile
{
char filename
int size
};


FILE*fp;
char fname[20];


void tar(){
printf("TAR FILE");
printf("Enter file name: ");
gets(fname);
fp=fopen(fname,"a");
if(fp==NULL){
printf("File is Error");
exit(0);
}
printf("TAR a File success");
return 0;
}



main(){
void tar();
}

