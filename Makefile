CC=gcc

all: server client

server: ftps.o lab2.o
	$(CC) ftps.o lab2.o -o server

client: ftpc.o lab2.o
	$(CC) ftpc.o lab2.o -o client

lab2.o: lab2.h
	$(CC) -c lab2.c -o lab2.o

ftps.o: lab2.h 
	$(CC) -c ftps.c -o ftps.o

ftpc.o: lab2.h 
	$(CC) -c ftpc.c -o ftpc.o

.PHONY: clean
clean:
	rm -rf server client recvd *.o
