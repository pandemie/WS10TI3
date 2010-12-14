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

    // do wc stuff
    char ch;
    int words, bytes, lines, inword;
    words = bytes = lines = inword = 0;

    while ( !feof ( fh ) ){
        // read a byte
        ch = fgetc ( fh );
        bytes += 1;

        // is it a newline?
        if ( ch == '\n' ){
            inword = 0;
            lines += 1;
        }

        // is it the beginning of a new word? (ignoring numbers)
        if ( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) ){
            if ( inword == 0 ){
                words += 1;
                inword = 1;
            }
        }

        // is it the ending of a word?
        if ( ch == ' ' ){
            inword = 0;
        }
    }
    fclose(fh);

    // print results
    printf ( "lines: %d\nwords: %d\nbytes: %d\n", lines, words, bytes );
    return 0;
}
