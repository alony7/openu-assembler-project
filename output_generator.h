#include "util_types.h"
#include "symbol_table.h"

Bool generate_output_files(char *object_filename, char *entry_filename, char *extern_filename,SymbolTable *relocation_table, SymbolTable *symbol_table, Word *code_image, Word *data_image);