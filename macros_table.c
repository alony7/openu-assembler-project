#include <string.h>
#include <malloc.h>
#include "macros_table.h"
#include "memory_wrappers.h"

/* Frees the memory associated with a single MacroItem */
static void free_macro_item(MacroItem *item);

MacroTable *create_macro_table() {
    /* Allocate memory for the table */
    MacroTable *table = (MacroTable *) safe_malloc(sizeof(MacroTable));
    table->capacity = 0;
    table->size = 0;
    table->items = NULL;
    return table;
}

MacroItem *create_macro_item(char *name, char **value, int line_number, int value_size) {
    /* Allocate memory for the item */
    MacroItem *item = (MacroItem *) safe_malloc(sizeof(MacroItem));
    /* Initialize the item */
    item->name = (char *) safe_malloc(strlen(name) + 1);
    strcpy(item->name, name);
    item->value = value;
    item->line_number = line_number;
    item->value_size = value_size;
    return item;
}

void add_macro_item(MacroTable *table, MacroItem *item) {
    /* If the table is full, increase its capacity*/
    if (table->size == table->capacity) {
        table->capacity += MACRO_TABLE_CAPACITY;
        /* Reallocate memory for the table */
        table->items = (MacroItem **) safe_realloc(table->items, table->capacity * sizeof(MacroItem *));
    }
    /* Add the item to the table */
    table->items[table->size] = item;
    table->size++;
}

void free_macro_item(MacroItem *item) {
    /* Free the name and value of the item */
    free(item->name);
    free_macro_item_value(item->value);
    /* Free the item itself */
    free(item);
}

MacroItem *get_macro_item(MacroTable *table, char *name) {
    int i;
    MacroItem *item = NULL;
    /* Iterate over the table to find the item */
    for (i = 0; i < table->size; i++) {
        item = table->items[i];

        /*  Check if the names match */
        if (strcmp(item->name, name) == 0) {
            return item;
        }
    }
    /* Item not found */
    return NULL;
}

void free_macro_table(MacroTable *table) {
    int i;

    for (i = 0; i < table->size; i++) {
        /* Free each individual MacroItem */
        free_macro_item(table->items[i]);
    }

    /*  Free the array of MacroItems */
    free(table->items);
    /* Free the MacroTable itself */
    free(table);
}

void append_macro_item_value(MacroItem *item, char *value_to_append) {
    int i;
    char **new_value;

    if (item->value == NULL) {
        /* First value being appended */
        item->value = (char **) safe_malloc(2 * sizeof(char *));
        item->value[0] = duplicate_string(value_to_append);
        item->value[1] = NULL;
    } else {
        /* Find the end of the value array */
        for (i = 0; item->value[i] != NULL; i++);

        /* Resize the value array to accommodate the new value */
        new_value = (char **) safe_realloc(item->value, (i + 2) * sizeof(char *));
        item->value = new_value;

        /* Append the new value */
        item->value[i] = duplicate_string(value_to_append);
        item->value[i + 1] = NULL;
    }

    item->value_size += 1;
}

void free_macro_item_value(char **value) {
    int i;

    if (value == NULL)
        return;

    for (i = 0; value[i] != NULL; i++) {
        /* Free each individual value string */
        free(value[i]);
    }

    /* Free the array of values */
    free(value);
}
