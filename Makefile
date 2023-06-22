CC = gcc
COMP_FLAG = -ansi -Wall -pedantic -std=c90

all: assembler.o
	$(CC) assembler.o io_parsers.o pre_process.o utils.o macros_table.o $(COMP_FLAG) $(DEBUG_FLAG) $(OBJS) -o $@

assembler.o: assembler.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

io_parsers.o: io_parsers.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

pre_process.o: pre_process.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

utils.o: utils.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

macros_table.o: macros_table.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@

clean:
	-rm assembler.o
