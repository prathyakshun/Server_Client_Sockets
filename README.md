# Computer Networks

**Prathyakshun Rajashankar 20161107**

**Anish Gulati 20161213**

### QUESTION 1

There are two directories in this question Client and Server containing the respective codes of client.c and server.c. 

**Error Handlers:**

- IP Address converting handler
- Socket creating and connection handler
- File availability handler

**Code Snipet:**

  ```bash
  # To run the server
  cd Q1/Server
  gcc server.c -o server
  ./server
  # Now the server is listening to the clients(if no errors in starting the server).
  ```

```bash
# To check the server-client connection
cd Q2/Client
gcc client.c -o client
./client
# Now all the files present on the server and available to us will be displayed 
# We will be asked to choose one among them
# Type the file name and the file will be downloaded
```

### QUESTION-2

#### 2_1: PERSISTANT CONNECTION

In this type of connection, the connection is maintained once it is established and all the files are transfered through this connection only. TCP Protocol is used in this code snipet and multiple files can be transfered as shown below:

**Code Snippet:**

```bash
cd Q2/Server
gcc server_persistant.c -o server_persistant
./server_persistant
# Now the server is listening to the clients(if no errors in starting the server).
# Don't close this terminal for the second part given below but open a new terminal.
```

```bash
# To check the server-client connection
cd Q2/Client
gcc client_persistant.c -o client_persistant
./client_persistant fileName1 fileName2 fileName3 fileName4
# The above file names will be checked if they are present in the "Data" folder of the server and downloaded if they are present
```

#### 2_2: NON-PERSISTANT CONNECTION

In this type of connection, every time a new file is requested or is sent from the server side, a new connection over the socket is opened. Here also the code uses the TCP Protocol.

**Code Snippet:**

```bash
cd Q2/Server
gcc server_persistant.c -o server_persistant
./server_persistant
# Now the server is listening to the clients(if no errors in starting the server).
# Don't close this terminal for the second part given below but open a new terminal.
```

```bash
# To check the server-client connection
cd Q2/Client
gcc client_nonpersistant.c -o client_nonpersistant
./client_nonpersistant fileName1 fileName2 fileName3 fileName4
# The above file names will be checked if they are present in the "Data" folder of the server and downloaded if they are present
```

- The Persistant connection proved to be faster than Non-Persistant connection when more than 100 files where transfered all together.