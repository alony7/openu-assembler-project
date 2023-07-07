#pragma once

#include "utils.h"
#include "instruction_handling.h"

/**
 * Represents a symbol in the symbol table.
 */
typedef struct Symbol {
    /* the name of the symbol */
    char *name;
    /* the address of the symbol in memory image */
    int address;
    /* the type of the symbol */
    InstructionType type;
} Symbol;

/**
 * Represents a symbol table.
 */
typedef struct SymbolTable {
    /* an array of pointers to symbols */
    Symbol **symbols;
    /* the number of symbols in the table */
    int size;
    /* the capacity of the table */
    int capacity;
} SymbolTable;

/**
 * Creates a new symbol table.
 *
 * @return The created symbol table.
 */
SymbolTable *create_symbol_table();

/**
 * Adds a symbol to the symbol table.
 *
 * @param table The symbol table.
 * @param symbol The symbol to add.
 * @return TRUE if the symbol was added successfully, FALSE otherwise.
 */
Bool add_symbol(SymbolTable *table, Symbol *symbol);

/**
 * Retrieves a symbol by name from the symbol table.
 *
 * @param table The symbol table.
 * @param name The name of the symbol to retrieve.
 * @return The symbol with the given name, or NULL if not found.
 */
Symbol *get_symbol(SymbolTable *table, char *name);

/**
 * Frees the memory allocated for a symbol table.
 *
 * @param table The symbol table to free.
 */
void free_symbol_table(SymbolTable *table);

/**
 * Frees the memory allocated for a symbol.
 *
 * @param symbol The symbol to free.
 */
void free_symbol(Symbol *symbol);

/**
 * Creates a new symbol with the given name, address, and type.
 *
 * @param name The name of the symbol.
 * @param address The address of the symbol.
 * @param type The type of the symbol.
 * @return The created symbol.
 */
Symbol *create_symbol(char *name, int address, InstructionType type);

/**
 * Counts the number of symbols in the symbol table with the specified type.
 *
 * @param table The symbol table.
 * @param type The type of symbols to count.
 * @return The number of symbols with the specified type.
 */
int count_symbols_by_type(SymbolTable *table, InstructionType type);
