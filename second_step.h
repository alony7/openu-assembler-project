#pragma once

#include "symbol_table.h"

/**
 * Perform the second step of the assembly process, which involves processing the remainig code lines
 * and generating the code image by resolving symbols and relocations.
 * This step is only for addresses that were not resolved in the first step (non immediate addressing).
 *
 * @param code_image The array to store the generated code image.
 * @param labels_table The symbol table containing defined labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param externals_table The symbol table containing external references in the code image.
 * @param errors The listnig of lines where processing was not successful in the first step.
 * @param file_name The name of the input file being processed.
 * @param ic Pointer to the instruction counter variable to track the current instruction address.
 *           This will be updated during the processing.
 *
 * @return TRUE if the second step processing is successful, FALSE otherwise.
 */
Bool second_step_process(Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table,ErrorList *errors,char *file_name, int *ic);