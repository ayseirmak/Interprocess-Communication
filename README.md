# Interprocess-Communication
Usage of Shared memory and  Message passing models of IPC
## Aim of the project
Simulating file and client process that A file server can handle 5 files with multiple clients on demand via shared memory.
### Server.c
It runs as a single process. It manages 5 files and shares them to clients over shared memory. First of all, to simulate this, it reads 5 text files from the disk and stores them as character arrays in its local memory (caching). This process takes client process id and requested file number (1, 2, 3, 4, 5) as input via a single mailbox. It works as multithreaded,so it runs a separate thread for each client process. The server process consists of the number of currently active clients plus a thread. The main thread listens to the maibox and assigns the relevant worker thread to fulfill the request from any client. The worker thread writes the requested numbered file content to the shared memory and sends a written signal to the client. Therefore, there are as many separate shared memories as there are clients. In that way, the shared memory of the clients with the server is separate. Main thread runs in an infinite loop.
### Client.c
Each client runs as a separate process in a single thread. Each client initially creates a shared memory. Then, in the loop, it transmits the desired file number (and shared memory address) to the server process over the mailbox. After the "operation complete" signal, it writes the file content from the  shared memory to the terminal. If the user does not want to continue, the loop is exited and the process terminates. When the process is terminated, the shared memory and the server thread which serving the process should also be terminated. It is recommended to run each client process in a separate terminal in order to prevent interfere of outputs.
## Implemented Libraries
Phtreads (Multithreaded programming) and IPC Mailbox
## Compile and Run
### Warning
Please reading files(f1.txt,f2.txt,f3.txt,f4.txt,f5.txt), server.c and client.c are found
in same directory.
### How to Compile
1.	Open a shell on the machine you wish to compile on.
2.	Change directory (cd) to the directory that contains "server.c" and "client.c".
3.	Enter the following command. 

```bash
gcc server.c -o <output_file_server> -lpthread
gcc client.c -o <output_file_client> -lpthread
```

### How to run
1.	Open a shell on the machine you wish to run on.
2.	Change directory (cd) to the directory that contains "server.c" and "client.c". 
3.	Enter the following command.

```bash
./<output_file_server>
./<output_file_client>
```
### Output
Enter the file no that you want to read its data.
