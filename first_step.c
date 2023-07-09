#include <string.h>
#include <malloc.h>
#include "first_step.h"
#include "instruction_handling.h"
#include "error.h"
#include "memory_wrappers.h"

/**
 * Adds the current instruction counter (IC) to all data addresses in the symbol table.
 *
 * @param table The symbol table to update.
 * @param ic The current instruction counter (IC) value.
 */
static void add_ic_to_all_data_addresses(SymbolTable *table, int ic);

/**
 * Handles the parsing of label definitions
 * will add the label to the symbol table, advance the parsed line to the next token and return TRUE.
 * this method does not address the instruction defined by the label. It only adds the label to the symbol table and advances the line.
 *
 * @param line The parsed line with the main operand being the label.
 * @param labels_table The symbol table for labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param ic Pointer to the current instruction counter (IC) value.
 * @param dc Pointer to the current data counter (DC) value.
 * @param next_instruction_type Pointer to store the instruction type of the label's value.
 * @return TRUE if the label parsing was successful, FALSE otherwise.
 */
static Bool parse_label(ParsedLine *line, SymbolTable *labels_table, SymbolTable *relocations_table, const int *ic, const int *dc, InstructionType *next_instruction_type);

/**
 * Handles the processing of a line by determining the instruction type and calling the appropriate functions for addressing and validation.
 *
 * @param labels_table The symbol table for labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param line The parsed line to process.
 * @param data_image The data image to update.
 * @param code_image The code image to update.
 * @param ic Pointer to the current instruction counter (IC) value.
 * @param dc Pointer to the current data counter (DC) value.
 * @return TRUE if the line processing was successful, FALSE otherwise.
 */
static Bool handle_line(SymbolTable *labels_table, SymbolTable *relocations_table, ParsedLine *line, Word *data_image, Word *code_image, int *ic, int *dc);


void add_ic_to_all_data_addresses(SymbolTable *table, int ic) {
    Symbol *symbol;
    int i;
    for (i = 0; i < table->size; i++) {
        symbol = table->symbols[i];
        /* only data and string symbols should be updated */
        if (symbol->type == DATA || symbol->type == STRING) {
            symbol->address += ic;
        }
    }

}

Bool parse_label(ParsedLine *line, SymbolTable *labels_table, SymbolTable *relocations_table, const int *ic,const int *dc, InstructionType *next_instruction_type) {
    Symbol *symbol = NULL;
    InstructionType label_instruction_type;
    /* remove the colon from the label name */
    line->main_operand[strlen(line->main_operand) - 1] = NULL_CHAR;
    /* check if the label name is not an instruction name */
    if(get_instruction_code(line->main_operand) != INVALID_INSTRUCTION) {
        throw_program_error(line->line_number, join_strings(2, "illegal label name: ", line->main_operand), line->file_name, TRUE);
        return FALSE;
    }
    /* check if the label is empty */
    if (line->parameters_count == 0) {
        throw_program_error(line->line_number, "empty label", line->file_name, FALSE);
        return FALSE;
    }
    /* check if the label has already been declared */
    if (get_symbol(labels_table, line->main_operand) != NULL) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->main_operand, "' has already been declared"), line->file_name, TRUE);
        return FALSE;
    }
    /* check if the label has already been declared as external */
    if ((symbol = get_symbol(relocations_table, line->main_operand)) != NULL && symbol->type == EXTERN) {
        throw_program_error(line->line_number, join_strings(3, "label '", line->main_operand, "' has already been declared as external"), line->file_name, TRUE);
        return FALSE;
    }
    /* extract the instruction type from the label's value */
    label_instruction_type = get_instruction_type(line->parameters[0]);
    switch (label_instruction_type) {
        case (DATA):
        case (STRING):
            /* add the label to the symbol table with dc as the address */
            symbol = create_symbol(line->main_operand, *dc, label_instruction_type);
            add_symbol(labels_table, symbol);
            break;
        case (COMMAND):
            /* add the label to the symbol table with ic as the address */
            symbol = create_symbol(line->main_operand, *ic, COMMAND);
            add_symbol(labels_table, symbol);
            break;
        default:
            /* if the labels value starts with a dot but it's not a data or string instruction, it's an invalid instruction type */
            if (line->parameters[0][0] == '.') {
                throw_program_error(line->line_number, join_strings(2, "invalid instruction type: ", line->parameters[0]), line->file_name, TRUE);
                return FALSE;
            }
    }
    /* advance the line to the next token */
    advance_line_operands(line);
    /* set the next instruction type to the label's value instruction type */
    *next_instruction_type = label_instruction_type;
    return TRUE;
}

Bool handle_line(SymbolTable *labels_table, SymbolTable *relocations_table, ParsedLine *line, Word *data_image, Word *code_image, int *ic, int *dc) {
    Symbol *symbol;
    char *validation_error[MAX_ERROR_LENGTH];
    Bool is_success = TRUE,instruction_is_label = FALSE;
    InstructionType instruction_type;
    /* check if the line is empty */
    if (line->main_operand == NULL) {
        return TRUE;
    }
    /* get the instruction type */
    instruction_type = get_instruction_type(line->main_operand);
    /* check if the instruction is a label */
    if (instruction_type == LABEL) {
        instruction_is_label = TRUE;
        /* parse the label */
        if (!parse_label(line, labels_table, relocations_table, ic, dc, &instruction_type)) {
            return FALSE;
        }
    }
    /* return true if the instruction is a comment or an empty line */
    if(instruction_type == COMMENT || instruction_type == EMPTY_LINE) {
        return TRUE;
    }
    /* we don't need to check the commas for a string instruction */
    if(instruction_type != STRING){
        /* check if the instruction's commas are valid */
        /* if the instruction is a label, the commas should be checked from the second token. otherwise, from the first token (which is the instruction itself) */
        if(!is_valid_commas(line->original_line, (char *) validation_error,instruction_is_label?2:1)) {
            throw_program_error(line->line_number, (char *) validation_error, line->file_name, FALSE);
            return FALSE;
        }
    }
    switch (instruction_type) {
        case (DATA):
            /* address the data instruction */
            CHECK_AND_UPDATE_SUCCESS(is_success, address_data_instruction(line, data_image, dc));
            break;
        case (STRING):
            /* address the string instruction */
            CHECK_AND_UPDATE_SUCCESS(is_success, address_string_instruction(line, data_image, dc));
            break;
        case (LABEL):
            /* if the instruction is a label than it's a nested label, since if the previous token was a label, it would have been parsed and advanced */
            throw_program_error(line->line_number, "nested labels are not allowed", line->file_name, FALSE);
            return FALSE;
        case (EXTERN):
            /* check if the label is already declared as entry or extern */
            if (get_symbol(relocations_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' is already declared as entry or extern"), line->file_name, TRUE);
                return FALSE;
            }
            /* check if the label is already defined in this file, which is illegal for extern labels */
            if (get_symbol(labels_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' cannot be external since it is defined in this file"), line->file_name, TRUE);
                return FALSE;
            }
            /* add the label to the symbol table with 0 as the address */
            symbol = create_symbol(line->parameters[0], 0, EXTERN);
            add_symbol(relocations_table, symbol);
            break;
        case (ENTRY):
            /* check if the label is already declared as entry or extern */
            if (get_symbol(relocations_table, line->parameters[0]) != NULL) {
                throw_program_error(line->line_number, join_strings(3, "label '", line->parameters[0], "' is already marked as extern or entry"), line->file_name, TRUE);
                return FALSE;
            }
            /* add the label to the symbol table with ic as the address */
            symbol = create_symbol(line->parameters[0], *ic, ENTRY);
            add_symbol(relocations_table, symbol);
            break;
        case (COMMAND):
            /* address as a command instruction */
            CHECK_AND_UPDATE_SUCCESS(is_success, address_code_instruction(line, code_image, ic));
            break;
        default:
            /* if the instruction is not a label, data, string, extern, entry or command, it's an invalid instruction */
            /* comments and empty lines are already handled before the switch */
            throw_program_error(line->line_number, join_strings(2, "invalid instruction: ", line->main_operand), line->file_name, TRUE);
            return FALSE;
    }

    return is_success;
}


Bool first_step_process(Word data_image[MEMORY_SIZE], Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, ErrorList *errors,char *file_name, int *ic, int *dc,Bool *should_second_step) {
    Bool is_line_success,is_file_success = TRUE;
    int lines_count = 1;
    ParsedLine current_line = {0};
    char line[MAX_LINE_LENGTH] = {0},tmp_line[MAX_LINE_LENGTH] = {0};
    /* open the file */
    FILE *file = create_file_stream(file_name, READ_MODE);
    /* check if the file was opened successfully */
    if (file == NULL) {
        return FALSE;
    }
    /* iterate over the file's lines */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        is_line_success = TRUE;
        /* check if the memory size is exceeded */
        if(is_memory_exceeded(*ic, *dc)) {
            throw_program_error(lines_count, "maximum memory size exceeded for current file" ,file_name, FALSE);
            /* TODO: rename this parameter */
            (*should_second_step) = FALSE;
            /* TODO: check if its necassary */
            add_error_line(errors, lines_count);
            return FALSE;
        }
        /* check if the line is too long */
        if (line[strlen(line) - 1] != '\n' && !feof(file)) {
            throw_program_error(lines_count, "line is too long", file_name, FALSE);
            (*should_second_step) = FALSE;
            /* TODO: check if its necassary */
            add_error_line(errors, lines_count);
            return FALSE;
        }
        strcpy(tmp_line, line);
        /* parse the line */
        parse_line(tmp_line, &current_line);
        current_line.line_number = lines_count;
        strcpy(current_line.file_name, file_name);
        /* process the line */
        CHECK_AND_UPDATE_SUCCESS(is_line_success,handle_line(labels_table, relocations_table, &current_line, data_image, code_image, ic, dc));
        /* if the line was not successful, add its number to the error list */
        if(!is_line_success) {
            add_error_line(errors, lines_count);
        }
        /* update the file's success flag */
        CHECK_AND_UPDATE_SUCCESS(is_file_success, is_line_success);
        lines_count++;
        /* free the line's memory */
        free_parsed_line(&current_line);
    }
    /* add the instruction counter to the data addresses */
    add_ic_to_all_data_addresses(labels_table, *ic);

    fclose(file);
    return is_file_success;
}

