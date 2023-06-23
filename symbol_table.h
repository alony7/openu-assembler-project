#pragma once


#include "utils.h"

typedef enum InstructionType {
    DATA,
    STRING,
    ENTRY,
    EXTERN,
    COMMAND,
    NONE
} InstructionType;

typedef enum AddressingType {
    IMMEDIATE,
    DIRECT,
    REGISTER,
    ERROR
} AddressingType;


typedef struct Symbol {
    char *name;
    int address;
    InstructionType type;
} Symbol;

typedef struct SymbolTable {
    Symbol *symbols;
    int size;
    int capacity;
} SymbolTable;

SymbolTable create_symbol_table();

Bool add_symbol(SymbolTable *table, Symbol *symbol);

Symbol *get_symbol(SymbolTable *table, char *name);

void free_symbol_table(SymbolTable *table);

void free_symbol(Symbol *symbol);

Symbol *create_symbol(char *name, int address, InstructionType type);



