//
// Created by alons on 23/06/2023.
//

#include <string.h>
#include "first_step.h"
#include "instruction_handling.h"
#include "error.h"


static void add_ic_to_all__data_addresses(SymbolTable *table, int ic) {
    Symbol *symbol;
    int i;
    for (i = 0; i < table->size; i++) {
        symbol = &(table->symbols[i]);
        if (symbol->type == DATA || symbol->type == STRING) {
            symbol->address += ic;
        }
    }

}

Bool parse_label(OperandRow *row, SymbolTable *labels_table, SymbolTable *relocations_table, int *ic, int *dc,
                 InstructionType *next_instruction_type) {
    Symbol *symbol = NULL;
    InstructionType label_instruction_type;
    row->operand[strlen(row->operand) - 1] = NULL_CHAR;
    if (row->parameters_count == 0) {
        export_error(row->line_number, "empty label", row->file_name);
        return FALSE;
    }
    if (get_symbol(labels_table, row->operand) != NULL) {
        export_error(row->line_number, join_strings(3, "label '", row->operand, "' has already been declared"),
                     row->file_name);
        return FALSE;
    }
    if ((symbol = get_symbol(relocations_table, row->operand)) != NULL && symbol->type == EXTERN) {
        export_error(row->line_number,
                     join_strings(3, "label '", row->operand, "' has already been declared as external"),
                     row->file_name);
        return FALSE;
    }
    label_instruction_type = get_instruction_type(row->parameters[0]);
    switch (label_instruction_type) {
        case (DATA):
        case (STRING):
            symbol = create_symbol(row->operand, *dc, label_instruction_type);
            add_symbol(labels_table, symbol);
            break;
        case (COMMAND):
            symbol = create_symbol(row->operand, *ic, COMMAND);
            add_symbol(labels_table, symbol);
            break;
        default:
            if (row->parameters[0][0] == '.') {
                export_error(row->line_number, join_strings(2, "invalid instruction type: ", row->parameters[0]),
                             row->file_name);
                return FALSE;
            }
    }
    advance_operand_row(row);
    *next_instruction_type = label_instruction_type;
    return TRUE;
}

Bool handle_row(SymbolTable *labels_table, SymbolTable *relocations_table, OperandRow *row, Word *data_image,
                Word *code_image, int *ic, int *dc) {
    int i;
    Symbol *symbol;
    Bool is_success = TRUE;
    InstructionType instruction_type;
    if (row->operand == NULL) {
        return TRUE;
    }
    instruction_type = get_instruction_type(row->operand);
    if (instruction_type == LABEL) {
        if (!parse_label(row, labels_table, relocations_table, ic, dc, &instruction_type)) {
            return FALSE;
        }
    };
    switch (instruction_type) {
        case (COMMENT):
        case (EMPTY_ROW):
            return TRUE;
        case (DATA):
            is_success = address_data_instruction(row, data_image, dc);
            break;
        case (STRING):
            is_success = address_string_instruction(row, data_image, dc);
            break;
        case (LABEL):
            export_error(row->line_number, "nested labels are not allowed", row->file_name);
        case (EXTERN):
            if (get_symbol(relocations_table, row->parameters[0]) != NULL) {
                export_error(row->line_number,
                             join_strings(3, "label '", row->parameters[0], "' is already declared as entry or extern"),
                             row->file_name);
                return FALSE;
            }
            if (get_symbol(labels_table, row->parameters[0]) != NULL) {
                export_error(row->line_number, join_strings(3, "label '", row->parameters[0],
                                                            "' cannot be external since it is defined in this file"),
                             row->file_name);
                return FALSE;
            }
            symbol = create_symbol(row->parameters[0], 0, EXTERN);
            add_symbol(relocations_table, symbol);
            break;
        case (ENTRY):
            if (get_symbol(relocations_table, row->parameters[0]) != NULL) {
                export_error(row->line_number,
                             join_strings(3, "label '", row->parameters[0], "' is already marked as extern or entry"),
                             row->file_name);
                return FALSE;
            }
            symbol = create_symbol(row->parameters[0], *ic, ENTRY);
            add_symbol(relocations_table, symbol);
            break;
        case (COMMAND):
            is_success = address_code_instruction(row, code_image, ic);
            break;
        default:
            export_error(row->line_number, join_strings(2, "invalid instruction: ", row->operand), row->file_name);
            return FALSE;
    }
    return is_success;
}


Bool first_step_process(SymbolTable labels_table, SymbolTable relocations_table, FileOperands *file_operands,
                        char *file_name) {
    int ic = MEMORY_OFFSET;
    int dc = 0;
    Word data_image[MEMORY_SIZE];
    Word code_image[MEMORY_SIZE];
    Bool is_success = TRUE;
    int i;
    OperandRow *current_row;
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    file_operands = parse_file_to_operand_rows(file, file_name);
    for (i = 0; i < file_operands->size; i++) {
        current_row = &file_operands->rows[i];
        current_row->line_number = i + 1;
        is_success = handle_row(&labels_table, &relocations_table, current_row, data_image, code_image, &ic, &dc) &&
                     is_success;
    }
    add_ic_to_all__data_addresses(&labels_table, ic);

    fclose(file);
    return is_success;
}

