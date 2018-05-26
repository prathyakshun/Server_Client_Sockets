#include<stdio.h>
#include<stdlib.h>

int main()
{
	char buffer[10000];
	FILE * fobj = fopen("Data/test.txt","r");
	fread(buffer, 1, 1000, fobj);
	printf("buffer is %s\n",buffer);
}