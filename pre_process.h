#pragma once

#include "util_types.h"

/**
 * Expand macros in the input file and write the result to the output file.
 * @param input_filename Name of the input file.
 * @param output_filename Name of the output file.
 * @return TRUE if macro expansion is successful, FALSE otherwise.
 */
Bool expand_file_macros(char *input_filename, char *output_filename);