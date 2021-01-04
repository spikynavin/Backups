#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    FILE*fp;
    char*Filepath = "/home/elinux1/Desktop/new.txt";
    //char*read_buffer[1024];
    char*read_buffer = "Hellolinux wrold\n";
    int buffersize = strlen(read_buffer)+1;

    char*readed_buffer = malloc(buffersize);

    if(readed_buffer==0){
        puts("Cant allocated memory\n");
    }
    fp = fopen(Filepath,"wb");
    if(fp){
        fwrite(read_buffer,buffersize,1,fp);
        puts("Buffer data Wrote to New file\n");
    }
    else{
        puts("Something worng to wrote\n");
    }
    fclose(fp);
}
