#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include<dirent.h>
#include <string.h>
#define PORT 8080
int main(int argc, char const *argv[])
{
    /* Variables */
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "file received from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    valread = read(new_socket, buffer, 1024);  /* read infromation received into the buffer */
    printf("%s\n\n",buffer );

    /* Get all the files available in the current directory */
    DIR *d;
    struct dirent *dir;
    d = opendir("./Data");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
 		{
 			if (strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0)	continue;
            printf("%s\n", dir->d_name);
 			strcat(dir->d_name, "\n");
            send(new_socket , dir -> d_name, strlen(dir -> d_name) , 0 );  // send the file names
        }
        closedir(d);
    }
    
    char buffer1[1024], buffer2[1024];
    buffer1[0]='D';	buffer1[1]='a';	buffer1[2]='t';	buffer1[3]='a';	buffer1[4]='/';
    valread = read(new_socket, buffer2, 1024);		/* read the file name required */
    buffer2[valread] = '\0';
    strcat(buffer1, buffer2);
    printf("\nRequested file name is %s\n",buffer2);
    FILE * fobj1 = fopen(buffer1,"r");
    if(fobj1 == NULL)
    {
		send(new_socket, "0", 1024, 0);
        fprintf(stderr, "File not found on the server\n");
        fprintf(stderr, "Server exiting\n");
        exit(1);
    }
	send(new_socket, "1", 1024, 0);

    /* Sending the file contents */
    char block_buffer[100010];
    int ret=0, chunksize=100000;
    while ((ret = fread(block_buffer, 1, chunksize, fobj1)) > 0)
    {
        block_buffer[ret] = '\0';
        if (send(new_socket, block_buffer, strlen(block_buffer), 0) < 0)
        {
            fprintf(stderr,"Failed to send file\n");
            return -1;
        }
    }

    printf("\nFile was successfully sent\n");
    fclose(fobj1);
    printf("Server exiting\n");
}
