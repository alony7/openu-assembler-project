//
// Created by alons on 23/06/2023.
//

#include "first_iter.h"

Bool first_iterate(FileOperands *file_operands, char *file_name) {
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    file_operands = parse_file_to_operand_rows(file);
    fclose(file);
    return TRUE;
}

