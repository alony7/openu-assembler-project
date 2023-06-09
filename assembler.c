#include <stdio.h>
#include "pre_process.h"


static void execute_program(int argc, char *argv[]){
    argv +=1;
    /*TODO: validate No args are given */


    /* TODO: invalid file names */

    /* run macro expand */
    expand_macros(argv,argc -1);


    /* run first step assemble */


    /* run second step assemble */
}

/*TODO: add detailed comment*/
int main(int argc, char *argv[]) {
    execute_program(argc,argv);
    return 0;
}
