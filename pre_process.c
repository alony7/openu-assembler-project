#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "consts.h"
#include "macros_table.h"
#include "io_parsers.h"



static Bool rewrite_macros(MacroTable *table,FILE *input_file, FILE *output_file, char *input_file_name);


Bool rewrite_macros( MacroTable *table,FILE *input_file, FILE *output_file, char *input_file_name) {
    int line_number = 0;
    Bool is_macro_line = FALSE;
    char *line_output = NULL;
    MacroItem *item = NULL, *tmp_item = NULL;
    ParsedLine parsed_line;
    MacroItem *current_item = NULL;
    char line[MAX_LINE_LENGTH] = {0},tmp_line[MAX_LINE_LENGTH] = {0};
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        strcpy(tmp_line, line);
        parse_line(tmp_line, &parsed_line);
        parsed_line.line_number = line_number;
        strcpy(parsed_line.file_name, input_file_name);
        if (!parsed_line.main_operand || (is_comment(line) && !is_macro_line)) {
            free_parsed_line(&parsed_line);
            continue;
        }
        if (strcmp(parsed_line.main_operand, END_MACRO_DIRECTIVE) == 0) {
            if (!is_macro_line) {
                throw_program_error(line_number, "invalid end of macro", parsed_line.file_name, FALSE);
                return FALSE;
            }
            if (parsed_line.parameters_count != 0) {
                throw_program_error(line_number, join_strings(2, END_MACRO_DIRECTIVE, " takes no parameters"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            is_macro_line = FALSE;
            add_macro_item(table, item);
            free_parsed_line(&parsed_line);
            continue;
        } else if (strcmp(parsed_line.main_operand, START_MACRO_DIRECTIVE) == 0) {
            if (parsed_line.parameters_count != 1) {
                throw_program_error(line_number, join_strings(2, START_MACRO_DIRECTIVE, " takes exactly 1 parameter"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            if (get_instruction_type(parsed_line.parameters[0]) != UNKNOWN) {
                throw_program_error(line_number, join_strings(2, "macro name cannot be an instruction name: ", parsed_line.parameters[0]), parsed_line.file_name, TRUE);
                return FALSE;
            }
            if (is_macro_line) {
                throw_program_error(line_number, "nested macros are not allowed", parsed_line.file_name, FALSE);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_line.parameters[0])) != NULL) {
                throw_program_error(line_number, join_strings(3, "macro '", parsed_line.parameters[0], "' already exists"), parsed_line.file_name, TRUE);
                return FALSE;
            }
            item = create_macro_item(parsed_line.parameters[0], NULL, line_number, 0);
            is_macro_line = TRUE;

        } else {
            if (is_macro_line) {
                append_macro_item_value(item, line);
            }else if ((current_item = get_macro_item(table, parsed_line.main_operand)) != NULL) {
                if (line_number < current_item->line_number) {
                    throw_program_error(parsed_line.line_number, join_strings(3, "macro '", current_item->name, "' is used before definition"), parsed_line.file_name, TRUE);
                    return FALSE;
                }
                line_output = join_string_array(current_item->value, current_item->value_size);
                fputs(line_output, output_file);
                free(line_output);
            } else {
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
    input_file = create_file_stream(input_filename, READ_MODE);
    output_file = create_file_stream(output_filename, WRITE_MODE);
    if (!(output_file && input_file)) {
        return FALSE;
    }
    macro_table = create_macro_table();
    fseek(input_file, 0, SEEK_SET);
    result = rewrite_macros(macro_table, input_file,output_file,input_filename);
    fclose(input_file);
    fclose(output_file);
    free_macro_table(macro_table);
    return result;
}



