#include <string.h>
#include "util_types.h"
#include "symbol_table.h"

/**
 * Rewrites dynamic word in the code image based on the addressing type and resolves labels.
 *
 * @param operand_type The addressing type of the operand.
 * @param ic Pointer to the current instruction counter (IC) value.
 * @param code_image The code image to update.
 * @param labels_table The symbol table for labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param externals_table The symbol table containing external references in the code image.
 * @param line The parsed line containing the operand.
 * @param parameter_index The index of the parameter containing the operand.
 * @return TRUE if rewriting and resolving were successful, FALSE otherwise.
 */
static Bool rewrite_dynamic_word(AddressingType operand_type, const int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, ParsedLine *line, int parameter_index);

/**
 * Processes a line by determining the instruction type and calling the appropriate functions for further processing.
 *
 * @param line The parsed line to process.
 * @param ic Pointer to the current instruction counter (IC) value.
 * @param code_image The code image to update.
 * @param labels_table The symbol table for labels.
 * @param relocations_table The symbol table containing external and entry declarations.
 * @param externals_table The symbol table containing external references in the code image.
 * @return TRUE if the line processing was successful, FALSE otherwise.
 */
static Bool process_line(ParsedLine *line, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table);


Bool rewrite_dynamic_word(AddressingType operand_type, const int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table, ParsedLine *line, int parameter_index) {
    Bool is_external = FALSE;
    Symbol *label_symbol = NULL,*relocation_symbol = NULL,*external_symbol = NULL;
    switch (operand_type) {
        case (NO_VALUE):
        case (IMMEDIATE):
        case (REGISTER):
            break;
        /* only labels need to be resolved */
        case (DIRECT):
            /* get the label symbol */
            label_symbol = get_symbol(labels_table, line->parameters[parameter_index]);
            /* if the symbol is in the relocations table, check if it is external */
            if ((relocation_symbol = get_symbol(relocations_table, line->parameters[parameter_index]))) {
                /* if the symbol is external, set the flag */
                is_external = relocation_symbol->type == EXTERN;
            }
            /* if the symbol was not defined and it is not external, throw an error */
            if (label_symbol == NULL && is_external == FALSE) {
                program_log(ERROR,line->line_number, join_strings(3, "label '", line->parameters[parameter_index], "' is not defined"), line->file_name, TRUE);
                return FALSE;
            }
            /* if the symbol is external, add it to the externals table and set the word to 0 */
            if (is_external) {
                external_symbol = create_symbol(line->parameters[parameter_index], *ic, EXTERN);
                add_symbol(externals_table, external_symbol);
                parse_symbol_to_word(0, &code_image[*ic], EXTERNAL_ADDRESSING);
            } else {
                /* otherwise, parse the symbol's address to the word */
                parse_symbol_to_word(label_symbol->address, &code_image[*ic], RELOCATABLE_ADDRESSING);
            }
            break;
    }
    return TRUE;
}

Bool process_line(ParsedLine *line, int *ic, Word *code_image, SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table) {
    Bool is_success = TRUE;
    Word *current_word = NULL;
    AddressingType src_operand_type = NO_VALUE;
    AddressingType dest_operand_type = NO_VALUE;
    /* if the line is empty, skip it */
    if (line->main_operand == NULL) {
        return TRUE;
    }
    switch (get_instruction_type(line->main_operand)) {
        case (ENTRY):
            /* check if the label is defined in this file scope. if not, throw an error */
            if (get_symbol(labels_table, line->parameters[0]) == NULL) {
                program_log(ERROR,line->line_number, join_strings(3, "entry '", line->parameters[0], "' is not defined in this file scope"), line->file_name, TRUE);
                return FALSE;
            }
            break;
        case (LABEL):
            /* examine the next word to determine the addressing types */
            advance_line_operands(line);
            /* process the label's code */
            process_line(line, ic, code_image, labels_table, relocations_table, externals_table);
            break;
        case (COMMAND):
            current_word = &code_image[*ic];
            get_word_addressing_types(current_word, &src_operand_type, &dest_operand_type);
            (*ic)++;
            /* if the command has no operands, than there is no need for further processing  (instruction word is already set from the first step) */
            if (src_operand_type == NO_VALUE && dest_operand_type == NO_VALUE) {
                return TRUE;
            }
            /* if both operands are registers, than they were already set in the first step (registers are addressed immediately, thus they were set in the first step) */
            if (src_operand_type == REGISTER && dest_operand_type == REGISTER) {
                (*ic)++;
                return TRUE;
            }
            /* if there is a source operand, process it */
            if(src_operand_type != NO_VALUE){
                ASSIGN_LEFT_LOGICAL_AND(is_success, rewrite_dynamic_word(src_operand_type, ic, code_image, labels_table, relocations_table, externals_table, line, 0));
                (*ic)++;
            }
            /* if there is a destination operand, process it */
            if(dest_operand_type != NO_VALUE){
                /* if there was no source operand, the destination operand is the first parameter. else, it is the second parameter */
                ASSIGN_LEFT_LOGICAL_AND(is_success, rewrite_dynamic_word(dest_operand_type, ic, code_image, labels_table, relocations_table, externals_table, line, src_operand_type == NO_VALUE ? 0 : 1));
                (*ic)++;
            }
        default:
            /* any other instruction types are not relevant for the second step */
            break;
    }
    return is_success;
}

Bool second_step_process(Word code_image[MEMORY_SIZE], SymbolTable *labels_table, SymbolTable *relocations_table, SymbolTable *externals_table,ErrorList *errors,char *file_name, int *ic) {
    Bool is_success = TRUE;
    int i = 0,lines_count = 1;
    ParsedLine parsed_line = {0};
    FILE *file = create_file_stream(file_name, "r");
    char line[MAX_LINE_LENGTH] = {0},tmp_line[MAX_LINE_LENGTH] = {0};
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        /* skip lines that are in error list */
        if(is_line_in_error_list(errors,lines_count)){
            lines_count++;
            continue;
        }
        /* parse line */
        strcpy(tmp_line, line);
        parse_line(tmp_line, &parsed_line);
        /* set parsed line data */
        parsed_line.line_number = lines_count;
        strcpy(parsed_line.file_name, file_name);
        /* process line */
        ASSIGN_LEFT_LOGICAL_AND(is_success, process_line(&parsed_line, ic, code_image, labels_table, relocations_table, externals_table));
        lines_count++;
        free_parsed_line(&parsed_line);
    }
    fclose(file);
    return is_success;
}
