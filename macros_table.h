#include "consts.h"
#include "utils.h"

typedef struct {
    char *name;
    char **value;
} MacroItem;

typedef struct {
    MacroItem *items;
    int capacity;
    int size;
} MacroTable;

MacroTable create_macro_table();

Bool add_macro_item(MacroTable *table, MacroItem *item);

MacroItem *remove_macro(MacroTable *table, char *macro_name);

MacroItem *get_macro_item(MacroTable *table, char *name);

void free_macro_table(MacroTable *table);

void free_macro_item(MacroItem *item);

MacroItem create_macro_item(char *name, char **value);

Bool add_append_item_value(MacroItem *item, char *value);