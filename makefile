CLASSES=
CC = gcc
CXXFLAGS= -g -Wall -pthread -lm
LIBS = -l_monitor
LIBSFLAGS = -L .

all: monitor producer consumer

monitor: $(CLASSES) monitor.o lib_monitor.a
	$(CC) $(LIBSFLAGS) -o $@  monitor.o $(CXXFLAGS) $(LIBS)

producer: $(CLASSES) producer.o lib_monitor.a
	$(CC) $(LIBSFLAGS) -o $@  producer.o $(CXXFLAGS) $(LIBS)

consumer: $(CLASSES) consumer.o lib_monitor.a
	$(CC) $(LIBSFLAGS) -o $@  consumer.o $(CXXFLAGS) $(LIBS)

monitor.o: monitor.c
	$(CC) -c -O $^

lib_monitor.o: lib_monitor.c
	$(CC) -c -O $^

producer.o: producer.c
	$(CC) -c -O $^

consumer.o: consumer.c
	$(CC) -c -O $^

lib_monitor.a: lib_monitor.o
	ar rcs $@ $^

clean:
	rm -rf *.o *.log *.gch *.swp *.dSYM *.tar.gz monitor.o monitor lib_monitor.a logfile producer consumer