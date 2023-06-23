#pragma once

#include "utils.h"

/* This function is the first iteration of the assembler.
 * It receives a file name and a pointer to a FileOperands struct.
 * It iterates over the file and parses it into the FileOperands struct.
 * It returns TRUE if the file is valid, FALSE otherwise.
 */
Bool first_iterate(FileOperands *file_operands, char *file_name);