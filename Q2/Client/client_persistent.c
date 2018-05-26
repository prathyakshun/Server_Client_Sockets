// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

/* All the constants go here */
#define BUFFER_SIZE 1024
#define PORT 8080

/* Function definitions */
void recieve_files(char *filename, long long file_size, int sock);
void socketRead(int sock, char *buffer, long long int size);
void concat(int n, char **argv);

/* Global Variables */
char block_buffer[100100], size_str[1212], allFiles[1212], check[1212];
int block_size;
char *fileNotFound = "File not found on the server\n";
char *fileFound = "File Transfer initiated\n";

int main(int argc, char *argv[])
{
    /* Variables */
    struct sockaddr_in serv_addr;
    int sock = 0;
    char *hello = "Hello from client";

    /* Creating socket for connection */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
    // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        fprintf(stderr, "\nConnection Failed \n");
        return -1;
    }

    if (argc == 1)
    {
        fprintf(stderr,"[Error]\nProvide the files names of the files to be downloaded\n");
        exit(0);
    }
    fprintf(stderr, "Connection ok\n");

    /* Sending the initial Hello to server */
    if (send(sock, hello, BUFFER_SIZE, 0) < 0){
        fprintf(stderr,"Failed to send Hello to server\n");
        exit(0);
    }

    /* Concatenate all the file names into one string and send to the server */
    concat(argc - 1, argv);
    fprintf(stderr, "concatenatedFiles: %s\n", allFiles);
    if(send(sock, allFiles, BUFFER_SIZE, 0) < 0){                        // Send file names
        fprintf(stderr, "Failed to send file names\n");
        exit(0);
    }
    char *files = strtok(allFiles, " ");

    /* Sending files one by one in the while loop */
    while(files != NULL){
        socketRead(sock, check, BUFFER_SIZE);
        if(check[0] == '0'){
            fprintf(stderr, "%s\n", fileNotFound);
            files = strtok(NULL, " ");
            continue;
        }
        else
            fprintf(stderr, "%s\n", fileFound);
        socketRead(sock, size_str, BUFFER_SIZE);
        fprintf(stderr, "FILE_SIZE: %s\n", size_str);
        recieve_files(files, atoi(size_str), sock);
        files = strtok(NULL, " ");
    }

    fprintf(stderr, "Files transferred successfully\n");
    fprintf(stderr, "Client exiting\n");
    return 0;
}

void recieve_files(char *filename, long long file_size, int sock){

    /* Variables */
    FILE * fobj = fopen(filename, "w");
    int j = 0, blocks = file_size / BUFFER_SIZE;

    /* While loop to write the buffer content into the specified file buffer by buffer */
    while (j < blocks)  
    {
        // Receive file contents
        socketRead(sock, block_buffer, BUFFER_SIZE);
        fwrite(block_buffer, sizeof(char), BUFFER_SIZE, fobj);
        j++;
    }

    long long int remaining = file_size % BUFFER_SIZE;
    socketRead(sock, block_buffer, remaining);
    fwrite(block_buffer, sizeof(char), remaining, fobj);
    fclose(fobj);
}

/* Function to concat all the file names into one final string */
void concat(int n, char **argv){
    int i, j, k;
    for(i = 1, j = 0;i < n; i++){
        char *a = argv[i];
        for(k = 0;k < strlen(a); k++){
            allFiles[j++] = a[k];
        }
        allFiles[j++] = ' ';
    }
    char *a = argv[n];
    for(k = 0;k < strlen(a); k++){
        allFiles[j++] = a[k];
    }
    allFiles[j] = '\0';
}

void socketRead(int sock, char *buffer, long long int size){
    if(read(sock, buffer, size) < 0){
        perror("Read Error");
        exit(0);
    }
}
