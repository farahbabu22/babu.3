#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "lib_monitor.h"

/*
 * Author: Farah Babu
 * SSO ID: FBKZX
 * email ID: fbkzx@umsystems.edu
 */

int countProducers = 2;
int countConsumers = 6;
int timeoutLimit = 100;
char logfile[64];
int shSMID; // shared semaphore id
int shBufferID; // shared buffer id for shared memory functions
int logID;
char *shLogptr;
int *shmptr;
pid_t *pidList;

extern int errno;

//To kill all processes
void killAll(){

    int i = 0;
    for(i =0; i < MAXPROCESS; i++){
        if(pidList[i] != 0){
            kill(pidList[i], SIGKILL);
        }
    }
    //exit(EXIT_SUCCESS);
}

//Run when the timer done
void timerDone(){
    killAll();
}

//To clean all processes
void cleanAll(){
    killAll();
    exit(EXIT_SUCCESS);
}


//remove memory for the semaphore
void clearSemaphore(){
    semctl(shSMID, 0, IPC_RMID, NULL);
}


//set the shared memory
void setSharedSemaphoreArray(){
    key_t shSEMKey = ftok("producer.c", 'a');

    shSMID = semget(shSEMKey, NUMSEMS, IPC_CREAT | 0666);

    if(shSMID == -1){
        perror("monitor.c: Error: Issue in semget creation..exiting");
        exit(EXIT_FAILURE);
    }

    semctl(shSMID, MUTEX, SETVAL, 1);
    semctl(shSMID, FREE_SPACE, SETVAL, 4);
    semctl(shSMID, BUFFER_IN, SETVAL, 0);
    semctl(shSMID, FREE_PROCESS, SETVAL, 19);
    semctl(shSMID, CONSUMERS_WAITING, SETVAL, countConsumers);
    semctl(shSMID, CONSUMERS_WORKING, SETVAL, countConsumers);
}

//clear log file shared memory
void clearLogFileShm(){
    if(shmdt(shLogptr) == -1){
        perror("monitor.c: Error: Failed to detached logfile shm");
    }
    
    if(shmctl(logID, IPC_RMID, NULL) == -1){
        perror("monitor.c: Error: Failed to remove shared buffer segment");
    }
}

//clear shared memory for the logfile name
void clearSharedMemory(){
    if(shmdt(shmptr) == -1){
        perror("monitor.c: Error: Failed to detach");
    }

    if(shmctl(shBufferID, IPC_RMID, NULL) == -1){
        perror("monitor.c: Error: Failed to remove shared buffer segment");
    }

}

//Shared buffer
void setBufferMemory(){
    key_t shBufferKey = ftok("consumer.c", 'a');

    shBufferID = shmget(shBufferKey, sizeof(int) * 6, IPC_CREAT | 0666);

    if(shBufferKey == -1){
        perror("monitor.c: Error: Issue in creating shared memory for buffer");
        clearSemaphore();
        exit(EXIT_FAILURE);
    }
    shmptr = (int *)shmat(shBufferID, 0, 0);

    if(shmptr == (int *) -1){
        perror("monitor.c: Error: Issue in attaching to shared memory");
        clearSemaphore();
        clearSharedMemory();
        exit(EXIT_FAILURE);
    }

    shmptr[NEXTIN] = 0;
    shmptr[NEXTOUT] = 0;
}

//Log file
void setLogFile(){
    key_t logKey = ftok("lib_monitor.c", 'a');

    logID = shmget(logKey, sizeof(char) * 64, IPC_CREAT | 0666);

    if(logID == -1){
        perror("monitor.c: Error: Issue with creating memory for sharing log file name");
        clearSharedMemory();
        clearSemaphore();
        exit(EXIT_FAILURE);
    }

    shLogptr = (char *)shmat(logID, 0, 0);

    if(shLogptr == (char *) -1){
        perror("monitor.c: Error: Issue in attaching to shared memory for log file");
        clearSemaphore();
        clearSharedMemory();
        exit(EXIT_FAILURE);
    }

    sprintf(shLogptr, "%s", logfile);



}

//Remove the process id
void removeProcessPID(pid_t p){
    int i = 0;
    for(i=0; i < MAXPROCESS; i++){
        if(pidList[i] == p){
            pidList[i] = 0;
            break;
        }
    }
}

//Clear child event
void childEvent(int signum){
    pid_t pid;
    while((pid = waitpid((pid_t)(-1), 0, WNOHANG)) > 0){
        removeProcessPID(pid);
        //semWait(CONSUMERS_WORKING);
        //semSignal(FREE_PROCESS);
    }
}

//Initalize all the processes
void initalizeProcessList(){
    int i = 0;
    pidList = malloc(sizeof(pid_t)*MAXPROCESS);
    for(i=0; i < MAXPROCESS; i++){
        pidList[i] = 0;
    }
}

//Get the empty index
int getEmptyProcessIndex(){
    int i = 0;
    for(i=0; i < MAXPROCESS; i++){
        if(pidList[i] == 0)
            return i;
    }
    return -1;
}




void displayHelpMenu(){
    printf("\ncommand line help\n monitor monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n");
    printf("\t-h            Describe how the project should be run and then, terminate.\n");
    printf("\t-o logfile    Name of the file to save logs; default: logfile.\n");
    printf("\t-p m          Number of producers; default: m = 2.\n");
    printf("\t-c n          Number of consumers; default: n = 6.\n");
    printf("\t-t time       The time in seconds after which the process will terminate, even if it has not finished. (Default: 100)\n");
    fflush(stdout);
}

//Check if producers fall in the limit
void checkProducers(){
    if (countProducers > MAXPROCESS - 1){
        //set producers to a lower limit
        countProducers = 12;
    }
}

//check if consumers are greater than the producers
void checkConsumers(){
    // ensuring consumers are more than producers resetting the value forcefully
    if(countProducers >= countConsumers){
        countConsumers = countProducers + 1;
    }
}

void setEventHandlers(){
    
    signal(SIGALRM, timerDone); //Timer done


    signal(SIGKILL, cleanAll);
    signal(SIGINT, cleanAll);

    struct sigaction sigAction;
    memset(&sigAction, 0, sizeof(sigAction));
    sigAction.sa_handler = childEvent;
    sigaction(SIGCHLD, &sigAction, NULL);
}

//fork
//excel
//To spawn off producer processes
void createProducers(){
    int i, position;
    for(i = 0; i < countProducers; i++){
        semWait(FREE_PROCESS);
        position = getEmptyProcessIndex();
        pidList[position] = fork();
        if(pidList[position] == 0){
            execl("./producer", "./producer", (char*)0);
        }
    }
}

//fork
//execl
//To spawn off child processes
void createConsumers(){
    int i, position;
    for(i = 0; i < countConsumers; i++){
        semWait(FREE_PROCESS);
        position = getEmptyProcessIndex();
        pidList[position] = fork();
        if(pidList[position] == 0){
            execl("./consumer", "./consumer", (char*)0);
        }
    }
}

int main(int argc, char *argv[]){
    int opt;



    setEventHandlers();



    if ( argc <= 1){
        errno = 22;
        perror("monitor: Error: improper arguments. To know proper aguments try -h\n");
        exit(EXIT_FAILURE);
        return 1;
    }

    strcpy(logfile, "logfile");
    
    
    while ((opt = getopt(argc, argv, "ho:p:c:t:")) != -1){
        switch(opt) {
            case 'h':
                    displayHelpMenu();
                    //printMonitor(); - testing monitor
                    return 0;
                    break;
            case 'o':
                    strcpy(logfile, optarg);
                    printf("\nmaking use of logfile:%s", logfile);
                    break;
            case 'p':
                    countProducers = atoi(optarg);
                    printf("\nThe producer count overridden is:%d", countProducers);
                    break;
            case 'c':
                    countConsumers = atoi(optarg);
                    printf("\nThe consumer count overridden is:%d", countConsumers);
                    break;
            case 't':
                    timeoutLimit = atoi(optarg);
                    printf("\nTimeout mentioned %d:", timeoutLimit);
                    alarm(timeoutLimit);
                    break;
            default:
                    perror("wrong arguments supplied.. try -h");
                    return 1;

        }
    }

    //printf("The maximum number of process %d", MAXPROCESS); DELETE

    checkProducers();
    checkConsumers();

    //printf("Producers %d, Consumers %d", countProducers, countConsumers); DELETE

    setSharedSemaphoreArray();
    setBufferMemory();
    setLogFile();



    initalizeProcessList();

    createProducers();

    createConsumers();
    printf("Set up completed");
    fflush(stdout);
    while(1){
        if(semctl(shSMID, CONSUMERS_WORKING, GETVAL, NULL) == 0){
            break;
        }
    }

    clearSharedMemory();
    clearSemaphore();
    clearLogFileShm();

    
    return 0;
}
