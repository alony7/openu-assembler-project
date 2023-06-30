
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "first_step.h"
#include "symbol_table.h"

static void execute_program(int argc, char *argv[]) {
    FileOperands *file_operands = NULL;
    SymbolTable label_symbol_table = create_symbol_table();
    SymbolTable relocations_symbol_table = create_symbol_table();

    /*TODO: extract to function*/
    if(argc == 1){
        printf("No arguments were given. Exiting...\n");
        exit(1);
    }

    /* run macro expand */
 /*    expand_macros(argv, argc - 1);*/


    /* run first step assemble */
    first_step_process(label_symbol_table, relocations_symbol_table,file_operands,
                       "C:\\Users\\alons\\vm\\exercises\\mm14\\openu-assembler-project\\test-files\\work1.am");

    /* run second step assemble */
}

/*TODO: add detailed comment*/
/* TODO: wrap malloc */
/* TODO: wrap free */
/* TODO: add pointer casting to all mallocs */
/* TODO: move function declaratios to head of headers */
/* TODO: remove export for internally used functions */
/* TODO: add string termination to all strings */
int main(int argc, char *argv[]) {
    execute_program(argc, argv);
    return 0;
}
