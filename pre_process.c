#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"
#include "input_utils.h"


static Bool line_is_directive(const char *line, const char *directive) {
    int i;
    for (i = 0; i < strlen(directive); i++) {
        if (line == NULL) {
            return FALSE;
        }
        if (line[i] == '\0') {
            return FALSE;
        }
        if (line[i] != directive[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

static Bool fill_macro_table(FileOperands *file_operands, MacroTable *table) {
    int row_number = 0;
    char *raw_line;
    OperandRow parsed_row;
    MacroItem *item;
    MacroItem *tmp_item = NULL;
    Bool is_macro = FALSE;
    for (row_number = 0; row_number < file_operands->size; row_number++) {
        parsed_row = file_operands->rows[row_number];
        raw_line = parsed_row.original_line;
        if (raw_line[0] == ';' && !is_macro) {
            continue;
        }
        if (line_is_directive(parsed_row.operand, END_MACRO_DIRECTIVE)) {
            if (!is_macro) {
                printf("Error: invalid end of macro in line %d\n", row_number);
                return FALSE;
            }
            is_macro = FALSE;
            add_macro_item(table, item);
            continue;
        } else if (line_is_directive(parsed_row.operand, START_MACRO_DIRECTIVE)) {
            if (parsed_row.parameters_count != 1) {
                printf("Error: invalid parameters for 'mcro' in line %d\n", row_number);
                return FALSE;
            }
            //TODO: validate macro doesnt already exist
            if (is_macro) {
                printf("Error: nested macro in line %d\n", row_number);
                return FALSE;
            }
            if ((tmp_item = get_macro_item(table, parsed_row.parameters[0])) != NULL) {
                printf("Error: macro '%s' already exists in line %d\n", tmp_item->name,tmp_item->row_number);
                return FALSE;
            }
            item = create_macro_item(parsed_row.parameters[0], NULL, row_number, 0);
            is_macro = TRUE;

        } else {
            if (is_macro) {
                append_macro_item_value(item, raw_line);
            }
        }

    }
    return TRUE;
}

//TODO: merge with fill to base file encoder
//function to expand the macros in every occurences, based on the macro table
static Bool rewrite_macros(FileOperands *file_operands, MacroTable *table, FILE *output_file) {
    int row_number = 0;
    Bool is_macro = FALSE;
    char *line_output = {0};
    char *raw_line = {0};
    OperandRow parsed_row;
    MacroItem *current_item = NULL;
    for (row_number = 0; row_number < file_operands->size; row_number++) {
        parsed_row = file_operands->rows[row_number];
        raw_line = line_output =  parsed_row.original_line;
        /* comment line */
        if (raw_line[0] == ';') {
            continue;
        } else if (line_is_directive(raw_line, START_MACRO_DIRECTIVE)) {
            is_macro = TRUE;
        } else if (line_is_directive(raw_line, END_MACRO_DIRECTIVE)) {
            //write macro value to output file
            is_macro = FALSE;
        }
            //append line to output file
        else {
            if (!is_macro) {

                //                //TODO: check if macro is valid
//                //TODO: check if macro is defined
//                //TODO: check if macro line is not before macro definition
                if (parsed_row.operand == NULL) {
                    line_output = raw_line;
                    }
                else if (get_macro_item(table, parsed_row.operand) != NULL) {
                    current_item = get_macro_item(table, parsed_row.operand);
                    line_output = string_array_to_string(current_item->value, current_item->value_size);

                } else {
                    line_output = raw_line;
                }
                fputs(line_output, output_file);
            }

            }
    }
}

static Bool expand_file_macros(char *filename) {
    Bool result = TRUE;
    FILE *input_file = fopen(filename, "r");
    //TODO: build output file name
    FILE *output_file = fopen("C:\\Users\\alons\\vm\\exercises\\mm14\\openu-assembler-project\\temp.txt", "w");
    MacroTable macro_table = create_macro_table();
    FileOperands *parsed_input_file = parse_file_to_operand_rows(input_file);
    if ((result = fill_macro_table(parsed_input_file, &macro_table))) {
        fseek(input_file, 0, SEEK_SET);
        result = rewrite_macros(parsed_input_file, &macro_table, output_file);
    }
    fclose(input_file);
    fclose(output_file);
    free_macro_table(&macro_table);
    return result;
}

Bool expand_macros(char *filenames[], int num_of_files) {
    int i;
    char filename_with_extension[256];
    FILE *current_file;
    for (i = 0; i < num_of_files; i++) {
        /* TODO: separate to new method */
        /* TODO: check buffer*/
        strcpy(filename_with_extension, filenames[i]);
        strcat(filename_with_extension, AS_FILE_EXTENSION);
        /*TODO: Output error message */
        if ((current_file = fopen(filename_with_extension, "r")) == NULL) {
            //print error message for failed file opening
            printf("Error: Failed to open file %s\n", filename_with_extension);
            continue;
        }
        /*TODO: expand macro */
        printf("Opening File: %s\n", filename_with_extension);
        expand_file_macros(filename_with_extension);
    }
    return TRUE;
}
