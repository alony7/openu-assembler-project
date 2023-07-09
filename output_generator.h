#pragma once
#include "util_types.h"
#include "symbol_table.h"

/**
 * Generates the object file, entries file, and externals file based on the given data.
 *
 * @param object_filename The name of the object file.
 * @param entry_filename The name of the entries file.
 * @param extern_filename The name of the externals file.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param symbol_table The symbol table for labels.
 * @param externals_table The symbol table containing external references in the code image.
 * @param code_image The code image.
 * @param data_image The data image.
 * @param dc Current data counter (DC) value.
 * @param ic Current instruction counter (IC) value.
 * @return TRUE if the output files were generated successfully, FALSE otherwise.
 */
Bool generate_output_files(char *object_filename, char *entry_filename, char *extern_filename, SymbolTable *relocation_table, SymbolTable *symbol_table, SymbolTable *externals_table, Word *code_image, Word *data_image, int dc, int ic);