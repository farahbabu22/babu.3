#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "lib_monitor.h"


void killProducer(){
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    srand(time(NULL));

    signal(SIGKILL, killProducer);
    int randomNumber = 0;

    do{
        produce();

        randomNumber = (rand() % 5);
        sleep(randomNumber);

    }while(1);
    return 0;
}
