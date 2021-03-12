#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "lib_monitor.h"


int countProducers = 2;
int countConsumers = 6;
int timeoutLimit = 100;
char *logfile = "logfile";

extern int errno;

void killAll(){
    exit(EXIT_SUCCESS);
}

void timerDone(){
    killAll();
}



void displayHelpMenu(){
    printf("\ncommand line help\n monitor monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n");
    printf("\t-h            Describe how the project should be run and then, terminate.\n");
    printf("\t-o logfile    Name of the file to save logs; default: logfile.\n");
    printf("\t-p m          Number of producers; default: m = 2.\n");
    printf("\t-c n          Number of consumers; default: n = 6.\n");
    printf("\t-t time       The time in seconds after which the process will terminate, even if it has not finished. (Default: 100)\n");
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
                    printMonitor();
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

        }
    }

    
    return 0;
}
