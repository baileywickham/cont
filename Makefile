CC = gcc
CFLAGS = -Wall
OBJS = cont.o contd.o

all: contd

contd: contd.c contd.h
	$(CC) $(CFLAGS) contd.c -o contd

clean:
	rm -f *.o cont contd daemon a.out
