#include <malloc.h>
#include <string.h>
#include "utils.h"

char *string_array_to_string(char **array, int size){
    int i;
    char *string = malloc(sizeof(char));
    string[0] = '\0';
    for(i=0;i<size;i++){
        string = realloc(string, (strlen(string) + strlen(array[i]) + 1) * sizeof(char));
        strcat(string, array[i]);
    }
    return string;
}

