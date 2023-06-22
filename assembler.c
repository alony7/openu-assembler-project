
#include <string.h>
#include "macros_table.h"
#include "pre_process.h"

static void execute_program(int argc, char *argv[]) {
    int i;
    argv += 1;


    /*TODO: extract to function*/
    if(argc == 1){
        printf("No arguments were given. Exiting...\n");
        return;
    }

    /*throw error for any filename given that ends with .as*/
    for( i = 0; i < argc -1; i++){
        if(strstr(argv[i],".as") != NULL){
            printf("Error: file name cannot end with .as\n");
            return;
        }
    }


    /* run macro expand */
    expand_macros(argv, argc - 1);


    /* run first step assemble */


    /* run second step assemble */
}

/*TODO: add detailed comment*/
/* TODO: wrap malloc */
int main(int argc, char *argv[]) {
    execute_program(argc, argv);
    return 0;
}
