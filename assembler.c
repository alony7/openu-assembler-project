
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "first_step.h"
#include "symbol_table.h"
#include "second_step.h"
#include "pre_process.h"

static void execute_program(int argc, char *argv[]) {
    Word data_image[MEMORY_SIZE];
    Word code_image[MEMORY_SIZE];
    char filename_with_as_extension[MAX_FILENAME_LENGTH];
    char filename_with_am_extension[MAX_FILENAME_LENGTH];
    FileOperands *file_operands = NULL;
    int i;
    int num_of_files = argc - 1;
    SymbolTable label_symbol_table = create_symbol_table();
    SymbolTable relocations_symbol_table = create_symbol_table();

    /*TODO: extract to function*/

    if(num_of_files <= 0){
        printf("No arguments were given. Exiting...\n");
        exit(1);
    }

    for(i=0;i<num_of_files;i++){
        build_output_filename(argv[i], AS_FILE_EXTENSION, filename_with_as_extension);
        build_output_filename(argv[i], AM_FILE_EXTENSION, filename_with_am_extension);
        printf("Opening File: %s\n", filename_with_as_extension);

        if (!expand_file_macros(filename_with_as_extension, filename_with_am_extension)) {
            throw_system_error("failed to expand file macros. skipping file...\n", filename_with_as_extension,FALSE);
            continue;
        }
        if(!first_step_process(data_image,code_image,&label_symbol_table, &relocations_symbol_table,&file_operands,filename_with_am_extension)) {
            throw_system_error("failed processing file. skipping file...\n", filename_with_as_extension,FALSE);
            continue;
        }
        if(!second_step_process(code_image,&label_symbol_table, &relocations_symbol_table,file_operands,filename_with_am_extension)) {
            throw_system_error("failed processing file. skipping file...\n", filename_with_as_extension,FALSE);
            continue;
        }
        printf("File %s processed successfully\n", filename_with_as_extension);
    }

    /* run first step assemble */
     ;
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


/*TODO: add detailed comment*/
/* TODO: wrap malloc */
/* TODO: wrap free */
/* TODO: add pointer casting to all mallocs */
/* TODO: move function declaratios to head of headers */
/* TODO: remove export for internally used functions */
/* TODO: add string termination to all strings */
/* TODO: remove .idea from git */
int main(int argc, char *argv[]) {
    execute_program(argc, argv);
    return 0;
}
