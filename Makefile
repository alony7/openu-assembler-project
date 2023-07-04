CC = gcc
COMP_FLAG = -ansi -Wall -pedantic -std=c90
SRCS = assembler.c io_parsers.c pre_process.c utils.c macros_table.c symbol_table.c error.c instruction_handling.c first_step.c second_step.c memory_wrappers.c output_generator.c
OBJS = $(SRCS:.c=.o)
TARGET = assembler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(COMP_FLAG)  -o $(TARGET)

clean:
	-rm *.o

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(TARGET) ./test-files/work1 ./test-files/work2; make clean