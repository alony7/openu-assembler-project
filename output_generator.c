#include "output_generator.h"

Bool generate_output_files(char *object_filename, char *entry_filename, char *extern_filename,
                           SymbolTable *relocation_table, SymbolTable *symbol_table, Word *code_image,
                           Word *data_image) {
    FILE *object_file, *entry_file, *extern_file;
    if(count_symbols_by_type(relocation_table,EXTERN)){
        /* TODO: create extern file */
    }
    if(count_symbols_by_type(relocation_table,ENTRY)){
        /* TODO: create extern file */
    }
}
