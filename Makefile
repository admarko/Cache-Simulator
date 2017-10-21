
CFLAGS = -g -Wall -Werror -std=c99
CC = gcc

csim: csim.c
	$(CC) $(CFLAGS) -o csim csim.c -lm

clean:
	rm -rf *.o
	rm -f csim
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
