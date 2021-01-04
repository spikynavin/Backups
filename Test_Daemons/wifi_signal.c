#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){
    int end, loop, line;
    char str[512],rssi[3];
    FILE *fd = fopen("/proc/net/wireless", "r");
    if (fd == NULL) {
        printf("Failed to open file\n");
        return -1;
    }
//    printf("Enter the line number to read : ");
//    scanf("%d", &line);
    line=3;

    for(end = loop = 0;loop<line;++loop){
        if(0==fgets(str, sizeof(str), fd)){//include '\n'
            end = 1;//can't input (EOF)
            break;
        }
    }
    if(!end)
        rssi[0]=str[21];
        rssi[1]=str[22];
        rssi[2]='\0';
        printf("%s", rssi);
    fclose(fd);

    return 0;
}


