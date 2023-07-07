#pragma once

#include "instruction_handling.h"
#include "util_types.h"

#define CHECK_AND_UPDATE_SUCCESS(is_success, instruction) (is_success = (instruction) && is_success)
#define CLEAN_STRING(str) do { clean_crlf(str); add_null_char(str); } while(0)

char *join_string_array(char **array, int size);

char *duplicate_string(const char *str);

Bool is_integer(char *str);

Bool is_comment(const char *operand);

Bool is_string_equals(const char *line, const char *directive);

Bool is_empty_line(char *line);

InstructionCode get_instruction_code(char *command);

Register get_register(char *operand);

Bool is_register(const char *operand);

Bool is_label(char *operand);

Bool is_memory_exceeded(int ic,int dc);

int parse_int(char *str);

char *join_strings(int num_strings, ...);

void trim_string_quotes(char *str);

void word_to_base64(Word *word, char *base64);

void clean_crlf(char *str);

void add_null_char(char *str);
