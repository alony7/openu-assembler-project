
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "first_step.h"
#include "symbol_table.h"
#include "second_step.h"

static void execute_program(int argc, char *argv[]) {
    Word data_image[MEMORY_SIZE];
    Word code_image[MEMORY_SIZE];
    FileOperands *file_operands = NULL;
    int i,j;
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
    if(first_step_process(data_image,code_image,&label_symbol_table, &relocations_symbol_table,&file_operands,
                       "C:\\Users\\alons\\vm\\exercises\\mm14\\openu-assembler-project\\test-files\\work1.am")){
        printf("first step success\n");
        second_step_process(data_image,code_image,&label_symbol_table, &relocations_symbol_table,file_operands,
                            "C:\\Users\\alons\\vm\\exercises\\mm14\\openu-assembler-project\\test-files\\work1.am");
        //pretty print the whole code image, with a line for every word
//        for(i = 100; i < 118; i++){
//            char word[200] = "";
//            for(j=0;j<=11;j++){
//                word[11-j] = (code_image[i].bits[j]) + '0';
//            }
//            printf("%s\n", word);
//        }
//        for(i = 0; i < 11; i++){
//            char word[200] = "";
//            for(j=0;j<=11;j++){
//                word[11-j] = (data_image[i].bits[j]) + '0';
//            }
//            printf("%s\n", word);
//        }



    }
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
