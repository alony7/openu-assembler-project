
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "first_step.h"
#include "symbol_table.h"
#include "second_step.h"
#include "pre_process.h"
#include "output_generator.h"

static void build_filenames(char *base_name, char *as_filename, char *am_filename, char *ob_filename, char *ext_filename,char *ent_filename);

void process_single_file(char* base_filename){
    Bool is_file_success = TRUE;
    Bool should_second_step = TRUE;
    int ic = MEMORY_OFFSET;
    int dc = 0;
    char as_filename[MAX_FILENAME_LENGTH] = {0}, am_filename[MAX_FILENAME_LENGTH] = {0}, ob_filename[MAX_FILENAME_LENGTH] = {0}, ext_filename[MAX_FILENAME_LENGTH] = {0}, ent_filename[MAX_FILENAME_LENGTH] = {0};
    ErrorList *errors = NULL;
    Word data_image[MEMORY_SIZE] = {0},code_image[MEMORY_SIZE] = {0};
    SymbolTable *label_symbol_table = NULL,*relocations_symbol_table = NULL,*externals_symbol_table = NULL;
    build_filenames(base_filename, as_filename, am_filename, ob_filename, ext_filename, ent_filename);
    fprintf(stdout,"Opening File: %s\n", as_filename);
    if (!expand_file_macros(as_filename, am_filename)) {
        throw_system_error(join_strings(3, "encountered errors while expanding macros. skipping file: '", as_filename, "' ..."), TRUE);
        return;
    }
    errors = create_error_list();
    label_symbol_table = create_symbol_table();
    relocations_symbol_table = create_symbol_table();
    CHECK_AND_UPDATE_SUCCESS(is_file_success, first_step_process(data_image, code_image, label_symbol_table, relocations_symbol_table, errors, am_filename, &ic, &dc, &should_second_step));
    if(!should_second_step){
        free_error_list(errors);
        throw_system_error(join_strings(3, "encountered errors while processing code. skipping file: '", am_filename, "' ..."), TRUE);
        return;
    }
    externals_symbol_table = create_symbol_table();
    ic = MEMORY_OFFSET;
    if(CHECK_AND_UPDATE_SUCCESS(is_file_success, second_step_process(code_image, label_symbol_table, relocations_symbol_table, externals_symbol_table,errors, am_filename, &ic))){
        CHECK_AND_UPDATE_SUCCESS(is_file_success, generate_output_files(ob_filename, ent_filename, ext_filename, relocations_symbol_table, label_symbol_table, externals_symbol_table, (Word *) &code_image, data_image, dc, ic));
    }
    if(is_file_success){
        fprintf(stdout,"File %s processed successfully\n", as_filename);
    }else{
        throw_system_error(join_strings(3, "encountered errors while processing code. skipping file: '", am_filename, "' ..."), TRUE);
    }
    free_symbol_table(label_symbol_table);
    free_symbol_table(relocations_symbol_table);
    free_symbol_table(externals_symbol_table);
    free_error_list(errors);
}

void build_filenames(char *base_name, char *as_filename, char *am_filename, char *ob_filename, char *ext_filename,char *ent_filename){
    build_output_filename(base_name, AS_FILE_EXTENSION, as_filename);
    build_output_filename(base_name, AM_FILE_EXTENSION, am_filename);
    build_output_filename(base_name, OB_FILE_EXTENSION, ob_filename);
    build_output_filename(base_name, EXT_FILE_EXTENSION, ext_filename);
    build_output_filename(base_name, ENT_FILE_EXTENSION, ent_filename);
}

void execute_program(int argc, char *argv[]) {
    int i;
    int num_of_files = argc - 1;

    /* TODO: add warn for as files */

    if (num_of_files <= 0) {
        fprintf(stdout,"No arguments were given. Exiting...\n");
        exit(1);
    }

    for (i = 1; i <= num_of_files; i++) {
        process_single_file(argv[i]);
    }

}


/*TODO: add detailed comment*/
/* TODO: add string termination to all strings */
/* TODO: validate label names are legal */
int main(int argc, char *argv[]) {
    execute_program(argc, argv);
    return 0;
}
