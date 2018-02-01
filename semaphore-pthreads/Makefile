# source files directory
SDIR=src

# compiler
CC=g++

# compiler flags 
CPPFLAGS=-g -Wall -Wextra -pthread -std=c++14 -O0

# make targets
all: semaphPthreads

semaphPthreads: semaphMain.o
	$(CC) $(CPPFLAGS) semaphMain.o -o semaphPthreads

semaphMain.o:
	$(CC) $(CPPFLAGS) -c $(SDIR)/semaphMain.cpp $(SDIR)/semaphMain.h

clean:
	rm -rf *.o semaphPthreads


