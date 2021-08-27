#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h> 
#include <signal.h>
#include <stdlib.h>
 
#define MSG_TYPE_A	10

int pidsignal;
int s=1;
int start =1;
int get_pid=0;
int thread_pid =0;
struct msgbuf 
{
    long mtype;       /* message type, must be > 0 */
    char* shmem;
    int file_no;    /* message data */
	int pid;
};

 
void handle_sigusr1(int sig){
	s=0;
}
void handle_sigusr2(int sig){
	pidsignal=0;
	get_pid=1;
	
}

int main(void)
{

	key_t key_buffer;
	key_buffer = ftok(".", 1);
	if(key_buffer == -1)
	{
		perror("get key buffer failed\n");
		return -1;
	}

	int msg_box;
 
	msg_box = msgget(key_buffer, IPC_CREAT|0666);
	if(msg_box == -1)
	{
		perror("get mailbox error\n");
		return -1;
	}
 
	key_t key_shmem;
	int no= getpid();
	key_shmem = ftok(".", no);
	if(key_shmem == -1)
	{
		perror("get key sharedmemory failed\n");
		return -1;
	}

	struct msgbuf msg;
	msg.mtype = MSG_TYPE_A;
	msg.pid = getpid();
	
	int shmid = shmget(key_shmem,250,0666|IPC_CREAT);
	msg.shmem=shmat(shmid,(const void*)0,0);
	if((int)msg.shmem==-1)
		printf("Shared Memmory attachment fail\n");
	else
	printf("Shared Memmory attachment successfully completed\n"); 
	
	struct sigaction sa_pid ={0};
	sa_pid.sa_handler= &handle_sigusr2;
	sigaction(SIGUSR2,&sa_pid,NULL);
	
	struct sigaction sa ={0};
	sa.sa_handler= &handle_sigusr1;
	sigaction(SIGUSR1,&sa,NULL);
	while(1)

	{
		printf("Enter file number: (1,2,3,4,5)\n");
		scanf("%d",&msg.file_no);
		msgsnd(msg_box, &msg, sizeof(msg), 0);

		if(thread_pid==0){
			pause();
			if(get_pid==1){
				thread_pid= atoi(msg.shmem);
				pidsignal=1;}
		}
		int answer;
		if(pidsignal==0)
			pause();
		pause();
		if(s==0){
			//printf("signal recived\n");
			printf("Reading shared memory: %s\n",msg.shmem);
			printf("Do you want to continue yes:1 / no:0 ?\n");
			scanf("%d",&answer);

		}
		if(answer==0){
			kill(thread_pid,SIGTERM);
			break;
		}
		else{
			s=1;
			kill(thread_pid,SIGUSR1);
			continue;	
		}
		
	}

	shmdt(msg.shmem);
    shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
