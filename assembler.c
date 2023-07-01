
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "first_step.h"
#include "symbol_table.h"
#include "second_step.h"
#include "pre_process.h"
#include "output_generator.h"

static void execute_program(int argc, char *argv[]) {
    int i;
    int ic,dc;
    Word data_image[MEMORY_SIZE];
    Word code_image[MEMORY_SIZE];
    char as_filename[MAX_FILENAME_LENGTH];
    char am_filename[MAX_FILENAME_LENGTH];
    char ob_filename[MAX_FILENAME_LENGTH];
    char ext_filename[MAX_FILENAME_LENGTH];
    char ent_filename[MAX_FILENAME_LENGTH];
    FileOperands *file_operands = NULL;
    int num_of_files = argc - 1;
    SymbolTable *label_symbol_table = NULL;
    SymbolTable *relocations_symbol_table = NULL;
    SymbolTable *externals_symbol_table = NULL;


    /*TODO: extract to function*/

    if(num_of_files <= 0){
        printf("No arguments were given. Exiting...\n");
        exit(1);
    }

    for(i=1;i<=num_of_files;i++){
        build_output_filename(argv[i], AS_FILE_EXTENSION, as_filename);
        build_output_filename(argv[i], AM_FILE_EXTENSION, am_filename);
        printf("Opening File: %s\n", as_filename);

        if (!expand_file_macros(as_filename, am_filename)) {
            throw_system_error(join_strings(3, "encountered errors while expanding macros. skipping file: '", as_filename, "' ..."), TRUE);
            continue;
        }
        label_symbol_table = create_symbol_table();
        relocations_symbol_table = create_symbol_table();
        ic = MEMORY_OFFSET;
        dc = 0;
        if(!first_step_process(data_image, code_image, label_symbol_table, relocations_symbol_table, &file_operands, am_filename,&ic,&dc)) {
            throw_system_error(join_strings(3, "encountered errors while processing code. skipping file: '", am_filename, "' ..."), TRUE);
            free_symbol_table(label_symbol_table);
            free_symbol_table(relocations_symbol_table);
            continue;
        }
        externals_symbol_table = create_symbol_table();
        ic = MEMORY_OFFSET;
        if(!second_step_process(code_image, label_symbol_table, relocations_symbol_table,externals_symbol_table, file_operands, &ic)) {
            throw_system_error(join_strings(3, "encountered errors while processing code. skipping file: '", am_filename, "' ..."), TRUE);
            free_symbol_table(label_symbol_table);
            free_symbol_table(relocations_symbol_table);
            free_symbol_table(externals_symbol_table);
            continue;
        }
        build_output_filename(argv[i], OB_FILE_EXTENSION, ob_filename);
        build_output_filename(argv[i], EXT_FILE_EXTENSION, ext_filename);
        build_output_filename(argv[i], ENT_FILE_EXTENSION, ent_filename);
        generate_output_files(ob_filename,ent_filename,ext_filename, relocations_symbol_table,label_symbol_table,externals_symbol_table, &code_image, data_image,dc,ic);
        printf("File %s processed successfully\n", as_filename);
    }

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
