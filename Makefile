#
# Student makefile for cs154 Project 3
#
# You should not modify this Makefile, or any file other than
# csim.c and trans.c: these two files alone contain all your work.
#
# For this project we require that your code compiles
# cleanly (without warnings), hence the -Werror option
CFLAGS = -g -Wall -Werror -std=c99
CC = gcc

all: csim test-trans tracegen

csim: csim.c cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o csim csim.c cachelab.c -lm

test-trans: test-trans.c trans.o cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o test-trans test-trans.c cachelab.c trans.o

tracegen: tracegen.c trans.o cachelab.c
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o cachelab.c

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
