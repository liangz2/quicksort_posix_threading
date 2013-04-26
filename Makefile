CC = gcc
CFLAGS = -Wall -pedantic -pthread -O3
LDFLAGS = -pthread

qs: qs.o
	$(CC) $(LDFLAGS) -o qs qs.o

qs.o: qs.c
	$(CC) $(CFLAGS) -c qs.c

clean:
	rm *.o
	rm qs