#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUF_SIZE 128

void* receiver(void*);//receiving function
int repeat = 1; //global variable for loops

typedef struct{
    long m_type; 
    char mtext[BUF_SIZE];
}mdata;


int main(int argc, char * argv[])
{
    pthread_t tid; //thread id
    pthread_attr_t attr; //set of thread attributes
    
    mdata d; 
    d.m_type = 1; 
    char str_buf[BUF_SIZE];
    int snd_key = 0, rcv_key = 0;
    int snd_queue = 0, rcv_queue = 0;
    
    if(argc < 3){
        perror("Need more arguments!");
        exit(1);
    }

    snd_key = atoi(argv[1]); //send key
    rcv_key = atoi(argv[2]); //receive key

    if(-1 == (snd_queue = msgget((key_t)snd_key, IPC_CREAT|0600))){
        perror("Failed to create snd_queue.");
        fprintf(stderr, "Select another key value.\n");
        exit(1);
    }
    if(-1 == (rcv_queue = msgget((key_t)rcv_key, IPC_CREAT|0600))){
        perror("Failed to create rcv_queue");
        fprintf(stderr, "Select another key value.\n");
        exit(1);
    }
    //printf("Parent thread(rcv_qid): %d\n", rcv_queue);

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, receiver, &rcv_queue); //create a child thread - receiver

    while(repeat)
    {
        printf("\n[msg to send] ");
        if(fgets(str_buf, BUF_SIZE, stdin) == NULL)
            fprintf(stderr, "Error, NULL received\n");
        else{
            size_t len = strlen(str_buf);
            char * pos;
            if((pos=strchr(str_buf, '\n')) != NULL){ //remove the trailing '\n'
                *pos = '\0';
                len--;
            }
        }
        if(!(strcmp(str_buf, "exit"))){
            repeat = 0;
            break;
        }
        else{ //fill in the d.mtext
            sprintf(d.mtext, "%s", str_buf);
        }

        //send message to snd_queue
        if(-1 == msgsnd(snd_queue, &d, sizeof(mdata) - sizeof(long), 0)){
            perror("msgsnd() failed");
            //exit(1);
        }
    }

    pthread_join(tid, NULL);
    
    msgctl(snd_queue, IPC_RMID, 0);
    msgctl(rcv_queue, IPC_RMID, 0);
    return 0;
}

void* receiver(void* param)
{
    mdata d;
    sprintf(d.mtext, "%s", "\0"); //clear the message buffer
    int *rcv_qid = (int*)param;   
    //printf("Child thread(rcv_qid): %d\n", *rcv_qid); 
    
    while(repeat)
    {
        if(-1 == msgrcv(*rcv_qid, &d, sizeof(mdata) - sizeof(long), 1, IPC_NOWAIT))
        {
            //perror("msgrcv() failed");
        }
        if(d.mtext[0] != '\0'){
            printf("\n[incoming] %s\n", d.mtext);
            sprintf(d.mtext, "%s", "\0"); //clear the message buffer
        }

        usleep(1000);
    }
    pthread_exit(0);
}
