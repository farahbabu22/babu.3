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


int countProducers = 2;
int countConsumers = 6;
int timeoutLimit = 100;
char *logfile = "logfile";
int shSMID; // shared semaphore id

extern int errno;

void killAll(){
    exit(EXIT_SUCCESS);
}

void timerDone(){
    killAll();
}

void cleanAll(){

}

void clearSemaphore(){
    semctl(shSMID, 0, IPC_RMID, NULL);
}

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



void displayHelpMenu(){
    printf("\ncommand line help\n monitor monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n");
    printf("\t-h            Describe how the project should be run and then, terminate.\n");
    printf("\t-o logfile    Name of the file to save logs; default: logfile.\n");
    printf("\t-p m          Number of producers; default: m = 2.\n");
    printf("\t-c n          Number of consumers; default: n = 6.\n");
    printf("\t-t time       The time in seconds after which the process will terminate, even if it has not finished. (Default: 100)\n");
    fflush(stdout);
}

void checkProducers(){
    if (countProducers > MAXPROCESS - 1){
        //set producers to a lower limit
        countProducers = 12;
    }
}

void checkConsumers(){
    // ensuring consumers are more than producers resetting the value forcefully
    if(countProducers >= countConsumers){
        countConsumers = countProducers + 1;
    }
}

int main(int argc, char *argv[]){
    int opt;



    signal(SIGALRM, timerDone); //Timer done


    if ( argc <= 1){
        errno = 22;
        perror("monitor: Error: improper arguments. To know proper aguments try -h\n");
        exit(EXIT_FAILURE);
        return 1;
    }
    
    
    while ((opt = getopt(argc, argv, "ho:p:c:t:")) != -1){
        switch(opt) {
            case 'h':
                    displayHelpMenu();
                    //printMonitor(); - testing monitor
                    return 0;
                    break;
            case 'o':
                    logfile = optarg;
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
    clearSemaphore();

    
    return 0;
}
