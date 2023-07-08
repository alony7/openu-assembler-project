#pragma once
#include "consts.h"
#include "utils.h"



/**
 * Structure representing a macro item.
 */
typedef struct {
    /* The name of the macro */
    char *name;
    /* The lines of the macro */
    char **value;
    /* The line number of the macro */
    int line_number;
    /* The amount of lines in the macro */
    int value_size;
} MacroItem;

/**
 * Structure representing a macro table.
 */
typedef struct {
    /* The items in the table */
    MacroItem **items;
    /* The amount of items that can be stored in the table */
    int capacity;
    /* The amount of items in the table */
    int size;
} MacroTable;

/**
 * Creates a new macro table.
 *
 * @return A pointer to the created macro table.
 */
MacroTable *create_macro_table();

/**
 * Adds a macro item to the macro table.
 *
 * @param table The macro table.
 * @param item The macro item to add.
 */
void add_macro_item(MacroTable *table, MacroItem *item);

/**
 * Retrieves a macro item from the macro table by name.
 *
 * @param table The macro table.
 * @param name The name of the macro item.
 * @return A pointer to the macro item if found, or NULL if not found.
 */
MacroItem *get_macro_item(MacroTable *table, char *name);

/**
 * Frees the memory occupied by the macro table.
 *
 * @param table The macro table to free.
 */
void free_macro_table(MacroTable *table);

/**
 * Creates a new macro item.
 *
 * @param name The name of the macro item.
 * @param value The value of the macro item.
 * @param line_number The line number where the macro was defined.
 * @param value_size The size of the macro item's value.
 * @return A pointer to the created macro item.
 */
MacroItem *create_macro_item(char *name, char **value, int line_number, int value_size);

/**
 * Appends a value to a macro item's value array.
 *
 * @param item The macro item.
 * @param value_to_append The value to append.
 */
void append_macro_item_value(MacroItem *item, char *value_to_append);

/**
 * Frees the memory occupied by a macro item's value array.
 *
 * @param value The value array to free.
 */
void free_macro_item_value(char **value);
