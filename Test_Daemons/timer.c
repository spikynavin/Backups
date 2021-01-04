#include <stdio.h>
#include <stdlib.h>

int main()
{
int data;
printf("Enter the data:");
scanf("%d",&data);
if(data==1)
{
	if(sleep(3)==0)
	{
		printf("Hello\n");
	}
}
return 0;
}
