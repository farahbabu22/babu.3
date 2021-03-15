#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "lib_monitor.h"

#define FREE_SPACE 1
#define BUFFER_IN 2
#define NEXTIN 4
#define NEXTOUT 5
#define NUMSEMS 7
#define MUTEX 0
#define FREE_PROCESS 19 
#define CONSUMERS_WAITING 3
#define CONSUMERS_WORKING 6
#define MAX_PRODUCERS 6
#define PRODUCERS_WORKING 5


int shSMID; // shared semaphore id
char *shLogptr; //shared memory for the log file
int logID; // shm id for the log file
int *bufferPtr; // semaphores
int shBufferID; // shm id memory for the buffer


//shmdt to detach shared memory
void detachMemory(){
    shmdt(bufferPtr);
    shmdt(shLogptr);
}

void cleanProducerConsumer(){
    detachMemory();
    exit(EXIT_SUCCESS);
}




//set up shared memory for both producer / consumer
void setSharedMem(){
    //key_t logKey = ftok();
    key_t shBufferKey = ftok("consumer.c", 'a');

    shBufferID = shmget(shBufferKey, sizeof(int) * 6, IPC_EXCL);
    if(shBufferID == -1){
        perror("lib_monitor.c: Error: Issue with creating memory for sharing buffer for producer");
        exit(EXIT_FAILURE);
    }

    bufferPtr = (int *)shmat(shBufferID, 0, 0);
    if(bufferPtr == (int *) -1){
        perror("lib_monitor.c: Error: Issue in attaching to shared memory");
        exit(EXIT_FAILURE);
    }


}

//set up shared memory for the log file for both producer / consumer
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


//Producer shared library
void produce(){
    setLogFileLib();

    semWait(CONSUMERS_WAITING);
    semSignal(PRODUCERS_WORKING);
    semWait(FREE_SPACE);
    semWait(MUTEX);

    srand(time(NULL));  //initalize random

    signal(SIGINT, cleanProducerConsumer);




    //Use to calcuate the time for the run
    time_t time_start, time_end;
    struct tm *time_start_info;
    struct tm *time_end_info;

    setSharedMem();


    FILE *logfileF;
    logfileF = fopen(shLogptr, "a");

    time(&time_start);
    time_start_info = localtime(&time_start);

    fprintf(logfileF, "%s : Producer process: Entering critical section", asctime(time_start_info));

    int randomnumber = (rand() % 5);
    sleep(randomnumber);

    int generatedNumber = (rand() % 100);

    bufferPtr[NEXTIN] = (bufferPtr[NEXTIN] + 1) % 4;

    time(&time_end);
    time_end_info = localtime(&time_end);


    fprintf(logfileF, "%s: Producer process: Exiting ciritical section with generated number: %d", asctime(time_end_info), generatedNumber);


    fclose(logfileF);


    detachMemory();

    semWait(PRODUCERS_WORKING);
    semSignal(MUTEX);
    semSignal(BUFFER_IN);

}


//Consumer shared library
void consume(){
    setLogFileLib();

    signal(SIGINT, cleanProducerConsumer);
    
    semWait(BUFFER_IN);
    semWait(MUTEX);


    time_t time_start, time_end;
    struct tm *time_start_info;
    struct tm *time_end_info;

    int readNum = 0;

    setSharedMem();

    readNum = bufferPtr[bufferPtr[NEXTOUT]];

    FILE *logfileF;
    logfileF = fopen(shLogptr, "a");

    fprintf(logfileF, "%s: The following was read by the consumer process(%d) %d:", asctime(time_start_info),  bufferPtr[NEXTOUT], readNum);

    int randomnumber = (rand() % 10);
    sleep(randomnumber);

    time(&time_end);
    time_end_info = localtime(&time_end);

    fprintf(logfileF, "%s: The following value has been consumed: %d", asctime(time_end_info), readNum);
    fclose(logfileF);

    detachMemory();

    semSignal(MUTEX);
    semSignal(FREE_SPACE);
    semWait(CONSUMERS_WORKING);


    
}

//generalizing the sem operations for sem buffer
void semOperation(int x, short ops){
        key_t shSEMKey = ftok("producer.c", 'a');
        shSMID = semget(shSEMKey, NUMSEMS, 0);

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

//called during semwait
void semWait(int x){
    semOperation(x, -1);
}

//called for sem signal
void semSignal(int x){
    semOperation(x, 1);
}


void printMonitor(){
    printf("\nMonitor is working\n");
}
