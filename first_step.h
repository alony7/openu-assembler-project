#pragma once

#include "utils.h"
#include "symbol_table.h"

Bool first_step_process(Word data_image[MEMORY_SIZE], Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table,ErrorList *errors,char *file_name, int *ic, int *dc,Bool *should_second_step);