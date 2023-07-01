#pragma once

#include "util_types.h"

void throw_program_error(int line_number, char *error_message, char *file_name, Bool should_free_smg);

void throw_system_error(char *error_message,char *file_name , Bool should_free_smg);