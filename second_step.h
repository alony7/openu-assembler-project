#include "symbol_table.h"

Bool second_step_process(Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, FileOperands *file_operands, int *ic);