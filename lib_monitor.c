#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include "lib_monitor.h"

int shSMID; // shared semaphore id
char *shLogptr;
int logID;
int *bufferPtr;
int shBufferID;





void setSharedMem(){
    //key_t logKey = ftok();
}

void setLogFileLib(){
    key_t logKey = ftok("lib_monitor.c", 'a');

    logID = shmget(logKey, sizeof(char) * 64, IPC_EXCL);

    if(logID == -1){
        perror("lib_monitor.c: Error: Issue with creating memory for sharing log file name");
        exit(EXIT_FAILURE);
    }

    shLogptr = (char *)shmat(logID, 0, 0);

    if(shLogptr == (char *) -1){
        perror("lib_monitor.c: Error: Issue in attaching to shared memory for log file");
        exit(EXIT_FAILURE);
    }
}

void produce(){
    setLogFileLib();

    semWait(CONSUMERS_WAITING);
    semSignal(PRODUCERS_WORKING);
    semWait(FREE_SPACE);
    semWait(MUTEX);

    srand(time(NULL));



}

void consume(){

}

void semOperation(int x, int ops){
        key_t shSEMKey = ftok("producer.c", 'a');
        shSMID = semget(shSEMKey, NUMSEMS, IPC_CREAT | 0666);

        if(shSMID == -1){
            perror("lib_monitor.c: Error: Issue in semget creation..exiting");
            exit(EXIT_FAILURE);
        }

        struct sembuf options;

        options.sem_num = x;
        options.sem_op = ops;
        options.sem_flg = 0;

        semop(shSMID, &options, 1);

}

void semWait(int x){
    semOperation(x, -1);
}

void semSignal(int x){
    semOperation(x, 1);
}


void printMonitor(){
    printf("\nMonitor is working\n");
}