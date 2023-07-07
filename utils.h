#pragma once

#include "instruction_handling.h"
#include "util_types.h"

#define CHECK_AND_UPDATE_SUCCESS(is_success, instruction) (is_success = (instruction) && is_success)
#define CLEAN_STRING(str) do { clean_crlf(str); add_null_char(str); } while(0)

/**
 * Concatenates an array of strings into a single string.
 *
 * @param array The array of strings.
 * @param size The size of the array.
 * @return The concatenated string.
 */
char *join_string_array(char **array, int size);

/**
 * Creates a duplicate of a given string.
 *
 * @param str The string to duplicate.
 * @return The duplicated string.
 */
char *duplicate_string(const char *str);

/**
 * Checks if a string represents a valid integer.
 *
 * @param str The string to check.
 * @return TRUE if the string is a valid integer, FALSE otherwise.
 */
Bool is_integer(char *str);

/**
 * Checks if a given operand is a comment.
 *
 * @param operand The operand to check.
 * @return TRUE if the operand is a comment, FALSE otherwise.
 */
Bool is_comment(const char *operand);

/**
 * Checks if a line is empty (contains only whitespace).
 *
 * @param line The line to check.
 * @return TRUE if the line is empty, FALSE otherwise.
 */
Bool is_empty_line(char *line);

/**
 * Gets the instruction code corresponding to a command.
 *
 * @param command The command string.
 * @return The instruction code.
 */
InstructionCode get_instruction_code(char *command);

/**
 * Gets the register corresponding to a register operand.
 *
 * @param operand The register operand string.
 * @return The register.
 */
Register get_register(char *operand);

/**
 * Checks if a given operand is a valid register.
 *
 * @param operand The operand to check.
 * @return TRUE if the operand is a valid register, FALSE otherwise.
 */
Bool is_register(const char *operand);

/**
 * Checks if an operand is a label.
 *
 * @param operand The operand to check.
 * @return TRUE if the operand is a label, FALSE otherwise.
 */
Bool is_label(char *operand);

/**
 * Checks if the total memory (instruction memory + data memory) exceeds the available memory size.
 *
 * @param ic The instruction counter.
 * @param dc The data counter.
 * @return TRUE if memory is exceeded, FALSE otherwise.
 */
Bool is_memory_exceeded(int ic, int dc);

/**
 * Parses an integer from a string.
 *
 * @param str The string to parse.
 * @param max_size The maximum allowed value for the integer.
 * @param min_size The minimum allowed value for the integer.
 * @param error_message The error message if parsing fails.
 * @return The parsed integer, or -1 if parsing fails.
 */
int parse_int(char *str, int max_size, int min_size, char **error_message);

/**
 * Concatenates multiple strings together into a single string.
 *
 * @param num_strings The number of strings to concatenate.
 * @param ... The variable arguments representing the strings.
 * @return The concatenated string.
 */
char *join_strings(int num_strings, ...);

/**
 * Trims quotes from the beginning and end of a string.
 *
 * @param str The string to trim.
 */
void trim_string_quotes(char *str);

/**
 * Converts a Word structure to Base64 representation.
 *
 * @param word The Word structure to convert.
 * @param base64 The output Base64 string.
 */
void word_to_base64(Word *word, char *base64);

/**
 * Cleans newline characters from a string.
 *
 * @param str The string to clean.
 */
void clean_crlf(char *str);

/**
 * Adds a null character to the end of a string.
 *
 * @param str The string to modify.
 */
void add_null_char(char *str);