#include <stdio.h>

int main (int argc, char** args){
    FILE* fh;

    // filename is an argument
    if ( argc == 2 ){
        // open file
        fh = fopen ( args[1], "r" );
        if ( fh == NULL ){
            fprintf ( stderr, "could not open '%s'\n", args[1] );
            return 1;
        }
    } else {
        // read from stdin
        fh = stdin;
    }

    // do cat stuff
    while ( !feof ( fh ) ){
        putc ( fgetc(fh), stdout );
    }
    fclose(fh);
    return 0;
}
