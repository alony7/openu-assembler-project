CC = gcc
COMP_FLAG = -ansi -Wall -pedantic -std=c90

all: assembler.o io_parsers.o pre_process.o utils.o macros_table.o symbol_table.o error.o instruction_handling.o first_step.o
	$(CC) assembler.o io_parsers.o pre_process.o utils.o macros_table.o error.o symbol_table.o instruction_handling.o first_step.o $(COMP_FLAG) $(DEBUG_FLAG) $(OBJS) -o assembler

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
symbol_table.o: symbol_table.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@
error.o: error.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@
instruction_handling.o: instruction_handling.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@
first_step.o: first_step.c
	$(CC) -c $(COMP_FLAG) $*.c -o $@
clean:
	-rm *.o
