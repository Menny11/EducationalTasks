# source files directory
SDIR=src

# compiler
CC=g++

# compiler flags 
CPPFLAGS=-g -Wall -Wextra -Wpedantic -pthread -std=c++14

# make targets
all: semaphPthreads

semaphPthreads: 
	$(CC) $(CPPFLAGS) $(SDIR)/semaphMain.cpp -o semaphPthreads

clean:
	rm -rf semaphPthreads


