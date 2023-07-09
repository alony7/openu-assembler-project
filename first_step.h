#pragma once

#include "utils.h"
#include "symbol_table.h"

/**
 * Processes the input file in the first step of the assembly process.
 * This involves processing the lines of the file, generating the data image and symbol tables,
 * and generating the code image with unresolved symbols and relocations.
 * Any errors encountered during the processing will be added to the error list.
 * Lines that can be immediately processed will be added to the code image/data image.
 *
 * @param data_image The data image to update.
 * @param code_image The code image to update.
 * @param labels_table The symbol table for labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param errors The error list to update.
 * @param file_name The name of the input file.
 * @param ic Pointer to the current instruction counter (IC) value.
 * @param dc Pointer to the current data counter (DC) value.
 * @param should_second_step Pointer to a flag indicating whether the second step should be performed or not.
 * @return TRUE if the first step processing was successful, FALSE otherwise.
 */
Bool first_step_process(Word data_image[MEMORY_SIZE], Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table,ErrorList *errors,char *file_name, int *ic, int *dc,Bool *should_second_step);