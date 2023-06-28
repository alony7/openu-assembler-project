#include "error.h"

void export_error(int line_number, char *error_message, char *file_name) {
    fprintf(stderr,"Error in file %s at line %d: %s\n", file_name, line_number, error_message);

}