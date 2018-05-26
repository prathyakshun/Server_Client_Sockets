#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <dirent.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define PORT 8080

/* Global Variables */
char *fileNotFound = "File not found on the server\n";
char *fileFound = "File Transfer initiated\n";

/* Function definitions */
int server_listen(int server_fd, struct sockaddr_in address, int addrlen);
int send_file(char *filename, int new_socket);
int socketRead(int sock, char *buffer, long long int size);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread, success;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

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
    new_socket = server_listen(server_fd, address, addrlen);

    /* Receiving the initial acknowledgement */
    socketRead(new_socket, buffer, BUFFER_SIZE);
    fprintf(stderr, "%s\n",buffer );

    /* Receiving the file names requested */
    valread = socketRead(new_socket, buffer, BUFFER_SIZE);
    buffer[valread] = '\0';
    char *files = strtok(buffer, " ");

    while(files != NULL){

        new_socket = server_listen(server_fd, address, addrlen);

        // Sending the file contents to the client
        success = send_file(files, new_socket);
        files = strtok(NULL, " ");
    }
    fprintf(stderr, "Server exiting\n");
}

int send_file(char *filename, int new_socket){

    /* Variables */
    long long int fd, size, j, blocks,i;
    char size_str[1010], block_buffer[100100],buffer2[1024], buffer3[1024];
    buffer3[0]='D'; buffer3[1]='a'; buffer3[2]='t'; buffer3[3]='a'; buffer3[4] = '/';
    for(i = 0;i < strlen(filename); i++)
        buffer3[5+i] = filename[i];
    /* Opening the file descriptor for the file to be sent */
    if((fd = open(buffer3, O_RDONLY)) < 0){
        fprintf(stderr, "%s\n", fileNotFound);
        send(new_socket, "0", BUFFER_SIZE, 0);  // "0" implies file not found(easy to check on client side)
        return 0; 
    }
    send(new_socket, "1", BUFFER_SIZE, 0);
    size = lseek(fd, 0, SEEK_END);

    /* Converting the file size into a string and sending it to client*/
    sprintf(size_str, "%lld", size);
    if(send(new_socket, size_str, BUFFER_SIZE, 0) < 0){
        perror("Send Error");
        exit(0);
    }
    j = 0; 
    blocks = (size / BUFFER_SIZE);
    while(j < blocks){
        long long int count = j * BUFFER_SIZE;
        lseek(fd, count, SEEK_SET);
        socketRead(fd, block_buffer, BUFFER_SIZE);

        if (send(new_socket, block_buffer, BUFFER_SIZE, 0) < 0){
            fprintf(stderr,"Failed to send file\n");
            exit(0);
        }
        j++;
    }
    long long int remaining = size % BUFFER_SIZE;
    lseek(fd, j * BUFFER_SIZE, SEEK_SET);
    socketRead(fd, block_buffer, remaining);

    if(send(new_socket, block_buffer, remaining, 0) < 0){
        fprintf(stderr,"Failed to send file\n");
        exit(0);
    }
    fprintf(stderr, "File was successfully sent\n");
    close(fd);
    return 1;   // meaning success
} 

int socketRead(int sock, char *buffer, long long int size){
    if(read(sock, buffer, size) < 0){ 
        perror("Read Error");
        exit(0);
    }   
}

int server_listen(int server_fd, struct sockaddr_in address, int addrlen){
    // The process is two step: first you listen(), then you accept()
    int new_socket;
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
    return new_socket;
}
