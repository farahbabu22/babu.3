#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "lib_monitor.h"

int shSMID; // shared semaphore id


void produce(){

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