#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "consts.h"
#include "macros_table.h"
#include "io_parsers.h"


/**
 * Rewrite macros in the input file, expanding them and writing the result to the output file.
 * @param table MacroTable representing the macro definitions.
 * @param input_file FILE pointer to the input file.
 * @param output_file FILE pointer to the output file.
 * @param input_file_name Name of the input file.
 * @return TRUE if macro rewriting and expansion is successful, FALSE otherwise.
 */
static Bool rewrite_macros(MacroTable *table,FILE *input_file, FILE *output_file, char *input_file_name);


Bool rewrite_macros( MacroTable *table,FILE *input_file, FILE *output_file, char *input_file_name) {
    int line_number = 0;
    Bool is_macro_line = FALSE;
    char *line_output = NULL;
    MacroItem *item = NULL, *tmp_item = NULL;
    ParsedLine parsed_line;
    MacroItem *current_item = NULL;
    char line[MAX_LINE_LENGTH] = {0},tmp_line[MAX_LINE_LENGTH] = {0};
    /* iterate over the lines in the input file */
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        strcpy(tmp_line, line);
        parse_line(tmp_line, &parsed_line);
        parsed_line.line_number = line_number;
        strcpy(parsed_line.file_name, input_file_name);
        /* if the line is a comment or empty, or if it's a macro line, skip it */
        if (!parsed_line.main_operand || (is_comment(line) && !is_macro_line)) {
            free_parsed_line(&parsed_line);
            continue;
        }
        /* if the line marks the end of a macro, add the current item to the table and continue */
        if (strcmp(parsed_line.main_operand, END_MACRO_DIRECTIVE) == 0) {
            if (!is_macro_line) {
                /* throw if the end macro directive is not preceded by a start macro directive */
                throw_program_error(line_number, "invalid end of macro", parsed_line.file_name, FALSE);
                return FALSE;
            }
            if (parsed_line.parameters_count != 0) {
                /* throw if the end macro directive has parameters */
                throw_program_error(line_number, join_strings(2, END_MACRO_DIRECTIVE, " takes no parameters"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            is_macro_line = FALSE;
            add_macro_item(table, item);
            free_parsed_line(&parsed_line);
            continue;
        }
        /* if the line marks the start of a macro, create a new item for it and continue */
        else if (strcmp(parsed_line.main_operand, START_MACRO_DIRECTIVE) == 0) {
            if (parsed_line.parameters_count != 1) {
                /* throw if there is no macro name */
                throw_program_error(line_number, join_strings(2, START_MACRO_DIRECTIVE, " takes exactly 1 parameter"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            if (get_instruction_type(parsed_line.parameters[0]) != UNKNOWN) {
                /* throw if the macro name is taken by an instruction */
                throw_program_error(line_number, join_strings(2, "macro name cannot be an instruction name: ", parsed_line.parameters[0]), parsed_line.file_name, TRUE);
                return FALSE;
            }
            if (is_macro_line) {
                /* throw if the start macro directive is inside another macro */
                throw_program_error(line_number, "nested macros are not allowed", parsed_line.file_name, FALSE);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_line.parameters[0])) != NULL) {
                /* throw if the macro name is already taken */
                throw_program_error(line_number, join_strings(3, "macro '", parsed_line.parameters[0], "' already exists"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            item = create_macro_item(parsed_line.parameters[0], NULL, line_number, 0);
            is_macro_line = TRUE;

        } else {
            if (is_macro_line) {
                /* the line is inside a macro, add it to the macro item */
                append_macro_item_value(item, line);
            }
            /* if the line is a macro call, expand it and write the result to the output file */
            else if ((current_item = get_macro_item(table, parsed_line.main_operand)) != NULL) {
                /* merge the lines of the macro item into a single string */
                line_output = join_string_array(current_item->value, current_item->value_size);
                fputs(line_output, output_file);
                free(line_output);
            } else {
                /* the line is not a macro call, write it to the output file as is */
                line_output = line;
                fputs(line_output, output_file);
            }
        }
        free_parsed_line(&parsed_line);
    }
    return TRUE;
}

Bool expand_file_macros(char *input_filename, char *output_filename) {
    FILE *input_file, *output_file;
    Bool result = TRUE;
    MacroTable *macro_table = NULL;
    /* open the input and output files */
    input_file = create_file_stream(input_filename, READ_MODE);
    output_file = create_file_stream(output_filename, WRITE_MODE);
    /* throw if the files could not be opened */
    if (!(output_file && input_file)) {
        return FALSE;
    }
    macro_table = create_macro_table();
    result = rewrite_macros(macro_table, input_file,output_file,input_filename);
    fclose(input_file);
    fclose(output_file);
    free_macro_table(macro_table);
    return result;
}



