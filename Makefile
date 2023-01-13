CFLAGS=-Wall -std=c99 -pedantic

all: Q1.exe Q2.exe

Q1.exe: Q1.o 
	gcc -o Q1 Q1.o

Q2.exe: Q2.o
	gcc -o Q2 Q2.o

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)