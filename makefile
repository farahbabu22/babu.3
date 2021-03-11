CLASSES=
CC = gcc
CXXFLAGS= -g -Wall -pthread

all: monitor

monitor: $(CLASSES)
	$(CC) -o $@  $^ $(CXXFLAGS) $@.c  -lm 

clean:
	rm -rf *.o *.log *.gch *.swp *.dSYM *.tar.gz monitor
