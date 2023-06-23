
#include <string.h>
#include <stdlib.h>
#include "macros_table.h"
#include "pre_process.h"

static void execute_program(int argc, char *argv[]) {
    int i;
    argv += 1;


    /*TODO: extract to function*/
    if(argc == 1){
        printf("No arguments were given. Exiting...\n");
        exit(1);
    }

    /* run macro expand */
    expand_macros(argv, argc - 1);


    /* run first step assemble */


    /* run second step assemble */
}

/*TODO: add detailed comment*/
/* TODO: wrap malloc */
/* TODO: wrap free */
/* TODO: errors to stderr */
/* TODO: add pointer casting to all mallocs */
int main(int argc, char *argv[]) {
    execute_program(argc, argv);
    return 0;
}
