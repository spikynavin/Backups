#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void main()
{
    FILE *fp;   
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
    printf("Enter the data \n");
    scanf("%s", data);
    fprintf(fp,"Data = %s\n", data);
    fclose(fp);
    fp = fopen(fname, "r");
    if (fp == NULL)
    {
    printf("Cannot open file \n");
    return;
    }
    ch = fgetc(fp);
    while (ch != EOF)
    {
    printf ("%c", ch);
    ch = fgetc(fp);
    }
    fclose(fp);
}
