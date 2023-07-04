#include <string.h>
#include <malloc.h>
#include "first_step.h"
#include "instruction_handling.h"
#include "error.h"
#include "memory_wrappers.h"

static Bool handle_line(SymbolTable *labels_table, SymbolTable *relocations_table, ParsedLine *line, Word *data_image, Word *code_image, int *ic, int *dc);

static Bool parse_label(ParsedLine *line, SymbolTable *labels_table, SymbolTable *relocations_table, const int *ic, const int *dc, InstructionType *next_instruction_type);

static void add_ic_to_all_data_addresses(SymbolTable *table, int ic);

static void add_ic_to_all_data_addresses(SymbolTable *table, int ic) {
    Symbol *symbol;
    int i;
    for (i = 0; i < table->size; i++) {
        symbol = table->symbols[i];
        if (symbol->type == DATA || symbol->type == STRING) {
            symbol->address += ic;
        }
    }

}

Bool parse_label(ParsedLine *line, SymbolTable *labels_table, SymbolTable *relocations_table, const int *ic,const int *dc, InstructionType *next_instruction_type) {
    Symbol *symbol = NULL;
    InstructionType label_instruction_type;
    line->main_operand[strlen(line->main_operand) - 1] = NULL_CHAR;
    if (line->parameters_count == 0) {
        throw_program_error(line->line_number, "empty label", line->file_name, FALSE);
        return FALSE;
    }
    if (get_symbol(labels_table, line->main_operand) != NULL) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->main_operand, "' has already been declared"), line->file_name, TRUE);
        return FALSE;
    }
    if ((symbol = get_symbol(relocations_table, line->main_operand)) != NULL && symbol->type == EXTERN) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->main_operand, "' has already been declared as external"), line->file_name, TRUE);
        return FALSE;
    }
    label_instruction_type = get_instruction_type(line->parameters[0]);
    switch (label_instruction_type) {
        case (DATA):
        case (STRING):
            symbol = create_symbol(line->main_operand, *dc, label_instruction_type);
            add_symbol(labels_table, symbol);
            break;
        case (COMMAND):
            symbol = create_symbol(line->main_operand, *ic, COMMAND);
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
    Symbol *symbol;
    char *validation_error[MAX_ERROR_LENGTH];
    Bool is_success = TRUE;
    InstructionType instruction_type;
    if (line->main_operand == NULL) {
        return TRUE;
    }
    instruction_type = get_instruction_type(line->main_operand);
    if (instruction_type == LABEL) {
        if (!parse_label(line, labels_table, relocations_table, ic, dc, &instruction_type)) {
            return FALSE;
        }
    }
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
            throw_program_error(line->line_number, join_strings(2, "invalid instruction: ", line->main_operand), line->file_name, TRUE);
            return FALSE;
    }
    if(!is_valid_commas(line->original_line, (char *) validation_error)) {
        throw_program_error(line->line_number, (char *) validation_error, line->file_name, FALSE);
        return FALSE;
    }
    return is_success;
}


Bool first_step_process(Word data_image[MEMORY_SIZE], Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, char *file_name, int *ic, int *dc) {
    Bool is_success = TRUE;
    int lines_count = 1;
    ParsedLine current_line = {0};
    char line[MAX_LINE_LENGTH] = {0},tmp_line[MAX_LINE_LENGTH] = {0};
    FILE *file = create_file_stream(file_name, "r");
    if (file == NULL) {
        return FALSE;
    }
    /* TODO: validate not infinite loop */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        strcpy(tmp_line, line);
        parse_line(tmp_line, &current_line);
        /*resize lines if needed*/
        current_line.line_number = lines_count;
        strcpy(current_line.file_name, file_name);
        CHECK_AND_UPDATE_SUCCESS(is_success, handle_line(labels_table, relocations_table, &current_line, data_image, code_image, ic, dc));
        lines_count++;
        free_parsed_line(&current_line);
    }
    add_ic_to_all_data_addresses(labels_table, *ic);

    fclose(file);
    return is_success;
}

