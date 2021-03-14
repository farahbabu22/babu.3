CLASSES=
CC = gcc
CXXFLAGS= -g -Wall -pthread -lm
LIBS = -l_monitor
LIBSFLAGS = -L .

all: monitor

monitor: $(CLASSES) monitor.o lib_monitor.a
	$(CC) $(LIBSFLAGS) -o $@  monitor.o $(CXXFLAGS) $(LIBS)

monitor.o: monitor.c
	$(CC) -c -O $^

lib_monitor.o: lib_monitor.c
	$(CC) -c -O $^

lib_monitor.a: lib_monitor.o
	ar rcs $@ $^

clean:
	rm -rf *.o *.log *.gch *.swp *.dSYM *.tar.gz monitor.o