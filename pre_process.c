#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"
#include "input_utils.h"


static Bool line_is_directive(const char *line, const char *directive) {
    int i;
    for (i = 0; i < strlen(directive); i++) {
        if (line[i] != directive[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

static Bool fill_macro_table(FILE *file, MacroTable *table) {
    char line[MAX_LINE_LENGTH] = {0};
    char tmp_line[MAX_LINE_LENGTH] = {0};
    OperandRow parsed_row;
    MacroItem *item;
    Bool is_macro = FALSE;
    int row_number = 0;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        row_number++;
        if (line[0] == '\n' || line[0] == ';') {
            continue;
        }
        strcpy(tmp_line, line);
        parse_operand_row(tmp_line, &parsed_row);
        /* comment line */
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
            item = create_macro_item(parsed_row.parameters[0], NULL, row_number, 0);
            is_macro = TRUE;

        } else {
            if (is_macro) {
                append_macro_item_value(item, line);
            }
        }
        //check if end of macro

    }
    return TRUE;
}
//TODO: merge with fill to base file encoder
//function to expand the macros in every occurences, based on the macro table
static Bool rewrite_macros(FILE *input_file, FILE *output_file, MacroTable *table) {
    char line[MAX_LINE_LENGTH] = {0};
    char tmp_line[MAX_LINE_LENGTH] = {0};
    char *line_output = NULL;
    OperandRow parsed_row;
    MacroItem *item;
    int row_number = 0;
    Bool is_macro = FALSE;
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        row_number++;
        if (line[0] == '\n' || line[0] == ';') {
            continue;
        }
        strcpy(tmp_line, line);
        parse_operand_row(tmp_line, &parsed_row);
        /* comment line */
        if (line[0] == ';') {
            continue;
        } else if (line_is_directive(line, START_MACRO_DIRECTIVE)) {
            is_macro = TRUE;
        } else if (line_is_directive(line, END_MACRO_DIRECTIVE)) {
            //write macro value to output file
            is_macro = FALSE;
        }
            //append line to output file
        else {
            if (!is_macro) {
                //                //TODO: check if macro is valid
//                //TODO: check if macro is defined
//                //TODO: check if macro line is not before macro definition
                if(get_macro_item(table,parsed_row.operand ) != NULL){
                    item = get_macro_item(table, parsed_row.operand );
                    line_output = string_array_to_string(item->value, item->value_size);

                }
                else{
                    line_output = line;
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
    if ((result = fill_macro_table(input_file, &macro_table))) {
        fseek(input_file, 0, SEEK_SET);
        result = rewrite_macros(input_file, output_file, &macro_table);
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
