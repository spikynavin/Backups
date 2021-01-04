#include <stdio.h>
#include <stdlib.h>

int main()
{
   int num;
   FILE *fptr;
   char buf[100];

   if ((fptr = fopen("/usr/present_backlight.txt","r")) == NULL){
       printf("Error opening file\n");
       exit(1);
   }
   fscanf(fptr,"%d", &num);
   printf("Backlight status= %d\n", num);
   snprintf(buf, sizeof(buf), "/usr/share/scripts/backlight %d",num);                                   
   system(buf); 
   fclose(fptr);
   return 0;
}
