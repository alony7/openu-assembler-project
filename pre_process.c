#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "consts.h"
#include "macros_table.h"

Bool expand_macros(char *filenames[], int num_of_files) {
    int i;
    char *new_filename;
    FILE *current_file;
    for(i=0;i<num_of_files;i++){
        /* TODO: separate to new method */
        /* TODO: check buffer*/
        new_filename = malloc(strlen(filenames[i]) + strlen(AS_FILE_EXTENSION));
        strcpy(new_filename,filenames[i]);
        strcat(new_filename,AS_FILE_EXTENSION);
        /*TODO: Output error message */
        if((current_file = fopen(new_filename, "r"))== NULL){
            free(new_filename);
            continue;
        }
        /*TODO: expand macro */
        printf("File: %s\n",new_filename);
        free(new_filename);
    }
    return TRUE;
}

static Bool expand_file_macros(char *filename){

}