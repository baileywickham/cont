CC = gcc
CFLAGS = -Wall
OBJS = cont.o contd.o

all: contd cont

cont: cont.c contd.h
	$(CC) $(CFLAGS) cont.c contd.o -o cont

contd: contd.c contd.h
	$(CC) $(CFLAGS) -c contd.c

clean:
	rm -f *.o cont contd
