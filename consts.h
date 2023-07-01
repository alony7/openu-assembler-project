#pragma once

#define AS_FILE_EXTENSION ".as"
#define AM_FILE_EXTENSION ".am"

/* TODO: update to 100 */
#define MEMORY_OFFSET 100
#define MEMORY_SIZE 1024
#define WORD_SIZE 12
#define START_MACRO_DIRECTIVE "mcro"
#define END_MACRO_DIRECTIVE "endmcro"
#define MAX_LINE_LENGTH 81
#define MACRO_TABLE_CAPACITY 10
#define INITIAL_SYMBOL_TABLE_CAPACITY 10
#define BASE_LINE_BATCH_SIZE 10
#define READ_MODE "r"
#define WRITE_MODE "w"
#define MAX_FILENAME_LENGTH 256
#define DATA_DIRECTIVE ".data"
#define STRING_DIRECTIVE ".string"
#define ENTRY_DIRECTIVE ".entry"
#define EXTERN_DIRECTIVE ".extern"
#define MOV_COMMAND "mov"
#define CMP_COMMAND "cmp"
#define ADD_COMMAND "add"
#define SUB_COMMAND "sub"
#define NOT_COMMAND "not"
#define CLR_COMMAND "clr"
#define LEA_COMMAND "lea"
#define INC_COMMAND "inc"
#define DEC_COMMAND "dec"
#define JMP_COMMAND "jmp"
#define BNE_COMMAND "bne"
#define RED_COMMAND "red"
#define PRN_COMMAND "prn"
#define JSR_COMMAND "jsr"
#define RTS_COMMAND "rts"
#define STOP_COMMAND "stop"
#define LABEL_TERMINATOR ':'
#define COMMENT_PREFIX ';'
#define REGISTER_PREFIX '@'
#define REGISTER_LETTER 'r'
#define NULL_CHAR '\0'
#define STRING_BOUNDARY '"'
#define ASCII_MAX 0x7f