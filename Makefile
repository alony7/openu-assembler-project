CC = gcc
COMP_FLAG = -ansi -Wall -pedantic -std=c90

all: assembler.o
	$(CC) assembler.o $(COMP_FLAG) $(DEBUG_FLAG) $(OBJS) -o $@

assembler.o: assembler.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

clean:
	-rm assembler.o
