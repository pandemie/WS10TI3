#include <stdio.h>

#define BOOL char
#define TRUE 1
#define FALSE 0

// Ist ein String in einem anderen String enthalten?
BOOL instr ( char* needle, char* haystack ){
    int i, j;
    for ( i = 0; haystack[i] != 0; ++i ){
        // Die beiden Strings vergleichen
        for ( j = 0; needle[j] == haystack[i+j]; ++j ){
            if ( needle[j+1] == 0 )
                return TRUE;
        }
    }
    return FALSE;
}

int main (int argc, char** args){
    FILE* fh;

    // there must be at least one argument
    if ( argc < 2 ){
        fprintf ( stderr, "I need a string to search for!\n" );
        return 1;
    }

    // filename is an argument
    if ( argc == 3 ){
        // open file
        fh = fopen ( args[2], "r" );
        if ( fh == NULL ){
            fprintf ( stderr, "could not open '%s'\n", args[2] );
            return 1;
        }
    } else {
        // read from stdin
        fh = stdin;
    }

    // do grep stuff
    char line [5096];
    while ( !feof ( fh ) ){
        // read line
        if ( fgets ( line, 5096, fh ) == NULL ){
            fprintf ( stderr, "error while reading file" );
            fclose(fh);
            return 2;
        }

        // print line (maybe)
        if ( instr ( args[1], line ) ){
            printf ( "%s", line );
        }
    }
    fclose(fh);
    return 0;
}
