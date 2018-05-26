// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

/* Contants definition */
#define PORT 8080

int main(int argc, char const *argv[])
{
    /* variables */
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0}, check[1024];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    fprintf(stderr,"Connection ok\n");                                        // Connection established
    
    if (send(sock, hello, strlen(hello), 0)  < 0)
    {
        fprintf(stderr,"Failed to send Hello message to the server\n");
        exit(0);
    }
    /* send the message. */
    printf("Hello message sent\n");

    char reqfile[100];
    int block_size, chunksize=100000;
    char block_buffer[100010];
    
    /* Get all the files available from the server */
    printf("Files available from the server:\n\n");
    block_size = read(sock, block_buffer, chunksize);
    block_buffer[block_size] = '\0';
    printf("%s\n",block_buffer);
    
    /* Ask user for the file to be downloaded */    
    printf("Which file would you like to be transferred?\n");
    scanf("%s",reqfile);
    send(sock, reqfile, strlen(reqfile), 0);                        /* Send file name */

	read(sock , check, 1024);
	if(check[0] == '0'){
		fprintf(stderr, "File not present on the server\n");
		exit(0);
	}

    FILE * fobj = fopen(reqfile, "w");
    
    /* Get all the file contents and write into the client location */
    while ((block_size = read(sock, block_buffer, chunksize)) > 0)
    {
        block_buffer[block_size] = '\0';
        fwrite(block_buffer, sizeof(char), block_size, fobj);
    }
    printf("\nFile transferred successfully\n");
    printf("\nClient exiting\n");
    fclose(fobj);
    return 0;
}
