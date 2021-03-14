#ifndef LIB_MONITOR_H
#define LIB_MONITOR_H

#define MAXPROCESS 19
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


void setSharedMem();
void setLogFileLib();
void produce();
void consume();
void semWait(int);
void semSignal(int);
void semOperation(int, int);
void printMonitor();

#endif