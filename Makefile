CC=gcc -std=c99 -pipe -Wall -pedantic -g
CFLAGS=-O2
LDFLAGS=-ludev

all: foodev

foodev: foodev.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

