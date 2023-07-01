#include <string.h>
#include "first_step.h"
#include "instruction_handling.h"
#include "error.h"


static void add_ic_to_all_data_addresses(SymbolTable *table, int ic) {
    Symbol *symbol;
    int i;
    for (i = 0; i < table->size; i++) {
        symbol = &(table->symbols[i]);
        if (symbol->type == DATA || symbol->type == STRING) {
            symbol->address += ic;
        }
    }

}

Bool parse_label(ParsedLine *line, SymbolTable *labels_table, SymbolTable *relocations_table, int *ic, int *dc, InstructionType *next_instruction_type) {
    Symbol *symbol = NULL;
    InstructionType label_instruction_type;
    line->operand[strlen(line->operand) - 1] = NULL_CHAR;
    if (line->parameters_count == 0) {
        throw_program_error(line->line_number, "empty label", line->file_name, FALSE);
        return FALSE;
    }
    if (get_symbol(labels_table, line->operand) != NULL) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->operand, "' has already been declared"), line->file_name, TRUE);
        return FALSE;
    }
    if ((symbol = get_symbol(relocations_table, line->operand)) != NULL && symbol->type == EXTERN) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->operand, "' has already been declared as external"), line->file_name, TRUE);
        return FALSE;
    }
    label_instruction_type = get_instruction_type(line->parameters[0]);
    switch (label_instruction_type) {
        case (DATA):
        case (STRING):
            symbol = create_symbol(line->operand, *dc, label_instruction_type);
            add_symbol(labels_table, symbol);
            break;
        case (COMMAND):
            symbol = create_symbol(line->operand, *ic, COMMAND);
            add_symbol(labels_table, symbol);
            break;
        default:
            if (line->parameters[0][0] == '.') {
                throw_program_error(line->line_number, join_strings(2, "invalid instruction type: ", line->parameters[0]), line->file_name, TRUE);
                return FALSE;
            }
    }
    advance_line_operands(line);
    *next_instruction_type = label_instruction_type;
    return TRUE;
}

Bool handle_line(SymbolTable *labels_table, SymbolTable *relocations_table, ParsedLine *line, Word *data_image, Word *code_image, int *ic, int *dc) {
    int i;
    Symbol *symbol;
    Bool is_success = TRUE;
    InstructionType instruction_type;
    if (line->operand == NULL) {
        return TRUE;
    }
    instruction_type = get_instruction_type(line->operand);
    if (instruction_type == LABEL) {
        if (!parse_label(line, labels_table, relocations_table, ic, dc, &instruction_type)) {
            return FALSE;
        }
    };
    switch (instruction_type) {
        case (COMMENT):
        case (EMPTY_LINE):
            return TRUE;
        case (DATA):
            CHECK_AND_UPDATE_SUCCESS(is_success, address_data_instruction(line, data_image, dc));
            break;
        case (STRING):
            CHECK_AND_UPDATE_SUCCESS(is_success, address_string_instruction(line, data_image, dc));
            break;
        case (LABEL):
            throw_program_error(line->line_number, "nested labels are not allowed", line->file_name, FALSE);
        case (EXTERN):
            if (get_symbol(relocations_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' is already declared as entry or extern"), line->file_name, TRUE);
                return FALSE;
            }
            if (get_symbol(labels_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' cannot be external since it is defined in this file"), line->file_name, TRUE);
                return FALSE;
            }
            symbol = create_symbol(line->parameters[0], 0, EXTERN);
            add_symbol(relocations_table, symbol);
            break;
        case (ENTRY):
            if (get_symbol(relocations_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' is already marked as extern or entry"), line->file_name, TRUE);
                return FALSE;
            }
            symbol = create_symbol(line->parameters[0], *ic, ENTRY);
            add_symbol(relocations_table, symbol);
            break;
        case (COMMAND):
            CHECK_AND_UPDATE_SUCCESS(is_success, address_code_instruction(line, code_image, ic));
            break;
        default:
            throw_program_error(line->line_number, join_strings(2, "invalid instruction: ", line->operand), line->file_name, TRUE);
            return FALSE;
    }
    /*TODO: validate every entry is in the table*/
    return is_success;
}


Bool first_step_process(Word data_image[MEMORY_SIZE], Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, FileOperands **file_operands, char *file_name, int *ic, int *dc) {
    Bool is_success = TRUE;
    int i;
    ParsedLine *current_line;
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    (*file_operands) = parse_lines_from_file(file, file_name);
    for (i = 0; i < (*file_operands)->size; i++) {
        current_line = &((*file_operands)->lines[i]);
        current_line->line_number = i + 1;
        CHECK_AND_UPDATE_SUCCESS(is_success, handle_line(labels_table, relocations_table, current_line, data_image, code_image, ic, dc));
    }
    add_ic_to_all_data_addresses(labels_table, *ic);

    fclose(file);
    return is_success;
}

