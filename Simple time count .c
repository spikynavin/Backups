#include <stdio.h>
int i;
int main(){
    for(i=0;i<10;i++)
    {
        printf("%d",i);
        printf("\033[A\33[2KT\rCount:%d\n",i);
        sleep(1);
        if (i==9)
        printf("Hello world");
    }

    return 0;
}
