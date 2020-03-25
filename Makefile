all:
	gcc -Wall cont.c -o cont
	gcc -Wall contd.c -o contd

clean:
	rm cont contd
