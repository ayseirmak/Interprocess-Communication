#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h> 
#include <pthread.h>
#include <signal.h>

#define MSG_TYPE	0
#define MSG_TYPE_A	10

int kill_thread=0;
void handle_sigterm(int sig){
	kill_thread=1;
}

void handle_usr1(int sig){
	kill_thread=0;
}
void input(char*,char*,char*,char*,char*);
void *worker(void *);
struct msgbuf 
{
    long mtype;       
    char* shmem;
    int file_no;    
	int pid;
};
 
char* file1;
char* file2;
char* file3;
char* file4;
char* file5;
 
int main(void)
{
	file1=malloc(250);
	file2=malloc(250);
	file3=malloc(250);
	file4=malloc(250);
	file5=malloc(250);
	input(file1,file2, file3, file4, file5);

	key_t key_buffer;
	key_buffer = ftok(".", 1);
	if(key_buffer == -1)
	{
		perror("get key failed\n");
		return -1;
	}
 
	int msg_box;
	msg_box = msgget(key_buffer, IPC_CREAT|0666);//If it does not exist, create it and set the permission to 0666
	if(msg_box == -1)
	{
		perror("get msg id error\n");
		return -1;
	}


	pthread_t main_thr;
	main_thr = pthread_self();
	while(1)
	{
		struct msgbuf msg;
		int retval;
		
            
		retval = msgrcv(msg_box,&msg, sizeof(msg), MSG_TYPE, IPC_NOWAIT);//IPC_NOWAIT, do not wait, that is, do not block	  
		 if((retval == -1)&&(errno != ENOMSG))//ENOMSG No information yet
		{
			perror("rcv error\n");
			return -1;
		}
		else if(retval != -1)
		{	printf("\n");
			printf("Mesage of Process:%d recived\n",msg.pid);

			printf("[PID:%d], msg type:%ld, msg:%d\n", msg.pid, msg.mtype, msg.file_no);

			struct msqid_ds msqid_ds , *buf;
			buf=& msqid_ds;
			msgctl( msg_box, IPC_STAT, buf);
			int current_msgnum=buf->msg_qnum;
			//int last_rcvmsg=buf->msg_lspid;
			//printf("Number of waiting process in mailbox:%d\n",current_msgnum);

			int thread_counter=current_msgnum+1;
			pthread_t tid[thread_counter];

			for(int i=0;i<thread_counter;i++){
			int thread = pthread_create(&tid[i], NULL, &worker, (void *) &msg);
			if (thread != 0)
				printf("\nThread could not be created  :[%s]", strerror(thread));
			}
		}
	}
	
	msgctl(msg_box, IPC_RMID, NULL);
 
	//return 0;
}
void *worker(void * msg)
{
	
	printf("You are inside worker thread\n");
   struct  msgbuf *msgw;   

   msgw = (struct msgbuf*) msg;
   int pid=msgw->pid;
   int file_no=msgw->file_no;
   
   key_t key_shmem;
	int no= pid;
	key_shmem = ftok(".", no);
	
	if(key_shmem == -1)
	{
		perror("get key sharedmemory failed\n");
		return -1;
	}
	int shmid = shmget(key_shmem,250,0666|IPC_CREAT);
	char* shmem=shmat(shmid,(const void*)0,0);
	if((int)shmem==-1)
		printf("Shared Memmory attachment fail\n");
	else
	printf("Shared Memmory attachment successfully completed\n");
	sprintf((char*)shmem,"%d",getpid()); 
	kill(pid,SIGUSR2);

	switch(file_no){
		case 1:
		strcpy((char*)shmem,file1); 
		break;
		case 2:
		strcpy((char*) shmem, file2);
		break;
		case 3:
		strcpy((char*) shmem, file3);
		break;
		case 4:
		strcpy((char*) shmem, file4);
		break;
		case 5:
		strcpy((char*) shmem, file5);
		break;
		default:
		printf("wong file no");
		key_shmem=-1;
	}
	printf("File%d content saccessfully copied to the shared memory\n",file_no);
	kill(pid,SIGUSR1);

	struct sigaction sa ={0};
	sa.sa_handler= &handle_sigterm;
	sigaction(SIGTERM,&sa,NULL);

	struct sigaction se ={0};
	se.sa_handler= &handle_usr1;
	sigaction(SIGUSR1,&se,NULL);


	pause();
	if(kill_thread==1){
		printf("Worker Thread is killed\n");
		shmdt(shmem);
    	shmctl(shmid,IPC_RMID,NULL);
		pthread_exit(NULL);	
	}

}

void input(char* file1,char* file2,char* file3,char* file4,char* file5){
	FILE *fp;
	long lSize;
//file1
	fp = fopen ( "f1.txt" , "rb" );
	if( !fp ) perror("f1.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );


	/* copy the file into the buffer */
	if( 1!=fread( file1 , lSize, 1 , fp) )
	fclose(fp),free(file1),fputs("entire read fails",stderr),exit(1);

	printf("file1 char array created\n");
	fclose(fp);
//file2
	fp = fopen ( "f2.txt" , "rb" );
	if( !fp ) perror("f2.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );


	/* copy the file into the buffer */
	if( 1!=fread( file2 , lSize, 1 , fp) )
	fclose(fp),free(file2),fputs("entire read fails",stderr),exit(1);

	printf("file2 char array created\n",file2);
	fclose(fp);
//file3
	fp = fopen ( "f3.txt" , "rb" );
	if( !fp ) perror("f3.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );


	/* copy the file into the buffer */
	if( 1!=fread( file3 , lSize, 1 , fp) )
	fclose(fp),free(file3),fputs("entire read fails",stderr),exit(1);

	printf("file3 char array created\n");
	fclose(fp);
//file4
	fp = fopen ( "f4.txt" , "rb" );
	if( !fp ) perror("f4.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );


	/* copy the file into the buffer */
	if( 1!=fread( file4 , lSize, 1 , fp) )
	fclose(fp),free(file4),fputs("entire read fails",stderr),exit(1);

	printf("file4 char array created\n");
	fclose(fp);
//File5
fp = fopen ( "f5.txt" , "rb" );
	if( !fp ) perror("f5.txt"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	/* copy the file into the buffer */
	if( 1!=fread( file5 , lSize, 1 , fp) )
	fclose(fp),free(file5),fputs("entire read fails",stderr),exit(1);

	printf("file5 char array created\n");
	fclose(fp);	
}
