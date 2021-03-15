#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "lib_monitor.h"

void killConsumer(){
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    srand(time(NULL));

    signal(SIGKILL, killConsumer);
    int randomNumber = 0;

    

    
    while(1){
        consume();
        randomNumber = (rand() % 10);
        sleep(randomNumber);
    }

    exit(EXIT_SUCCESS);
}
