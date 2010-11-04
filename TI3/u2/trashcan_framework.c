/* trashcan.c */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE         1024
#define PATHSIZE        256
#define TRASHFOLDER ".ti3_trash"


char copy_buffer[BUFSIZE];

int remove_file(char*,char*);

/* 'copy' kopiert den Dateiinhalt einer Datei namens
 * "source". Eine Kopie target wird nur erzeugt, wenn
 * eine Datei "target" noch nicht existiert.
 * Die Zugriffsrechte werden nicht kopiert, sondern
 * auf "rwx --- ---" gesetzt.
 */
#define COPY_SUCCESS        0
#define COPY_FILE_EXISTS    1
#define COPY_NO_SOURCE      2
#define COPY_ERROR          3
int copy(char *source, char *target)
{
    int fd_source, fd_target, tmp;
    int returnval = COPY_ERROR;

    // open source
    fd_source = open(source, O_RDONLY);
    if(fd_source == -1){
        returnval = COPY_NO_SOURCE;
    } else {
        // open target
        fd_target = open(target, O_APPEND | O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);
        if ( fd_target == -1){
            returnval = errno==EEXIST ? COPY_FILE_EXISTS : COPY_ERROR;
        } else {
            // copy
            while(1){
                tmp = read(fd_source, copy_buffer, BUFSIZE);
                if ( tmp == 0 ){ // EOF
                    returnval = COPY_SUCCESS;
                    break;
                } else if ( tmp == -1 ) { // ERROR
                    break;
                } else {
                    // write
                    if ( write(fd_target, copy_buffer, tmp) == -1 ){
                        break;
                    }
                }
            }

        }
    }

    // close
    close(fd_target);
    close(fd_source);
    return returnval;
}


char parse_command(char *command)
{
    return command[1];
}


/* erzeugt einen Ordner foldername */
int setup_trashcan(char *foldername)
{
  return (int) mkdir(foldername,0777);
}

/* fügt foldername und filename zu einem pfad zusmmen */
void merge_path(char *foldername, char *filename, char *path){
  while(*foldername != 0){
    *(path++) = *(foldername++);
  }
  *(path++) = '/';
  while(*filename != 0){
    *(path++) = *(filename++);
  }
  *(path++) = 0;
}

/* führt trashcan -p[ut] filename aus */
int put_file(char *foldername, char *filename)
{
  //foldername und filename weden zu "foldername/filename" in path[]
  char path[PATHSIZE];
  merge_path(foldername, filename, path);

  int tmp, i;
  tmp = copy(filename, path);
  if ( tmp == COPY_NO_SOURCE ){
    return -1;
  } else if ( tmp == COPY_ERROR ) {
	  return -2;
  } else if ( tmp == COPY_FILE_EXISTS ){
	  // Die Datei existiert im trash schon
	  // Also umbenennen in "filename (2)"
	  for ( i = 2; 1; ++i ){
		  sprintf ( path, "%s/%s (%d)", foldername, filename, i );
		  if ( copy(filename, path) == COPY_SUCCESS)
			  break;
		  if ( i == 1024 )
			  return -2;
	  }
  }

  char dot[] = ".";
  if (remove_file(dot, filename)  == 1){
    return -3;
  }

  return 0;
}


/* führt trashcan -g[et] filename aus */
int get_file(char *foldername, char *filename)
{
    //in path1 steht schließlich foldername/filename
    //in path2 steht schließlich ./filename
    char path1[PATHSIZE];
    char path2[PATHSIZE];
    char dot[] = ".";
    merge_path(foldername,filename,path1);
    merge_path(dot,filename,path2);
    switch ( copy(path1,path2) ){
        case COPY_SUCCESS:
            if ( remove_file(dot,path1) )
                return -3;
            else
                return 0;

        case COPY_NO_SOURCE:
            return -1;

        case COPY_FILE_EXISTS:
        case COPY_ERROR:
        default:
            return -2;
    }
}

/* führt trashcan -r[emove] filename aus */
/* return values:   0: success
 *                  1: error            */
int remove_file(char *foldername, char *filename)
{
  char path[PATHSIZE];
  int result;
  merge_path(foldername, filename, path);
  result = unlink(path);
  return result == 0 ? 0 : 1;
}


int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("...not enough arguments!\n");
        return EXIT_FAILURE;
    } else {
        setup_trashcan(TRASHFOLDER);
        char command = parse_command(argv[1]);
        switch (command) {
            case 'p':
                if (argc != 3) {
                    printf("...not enough arguments!\n");
                    return EXIT_FAILURE;
                }
                int p_error = put_file(TRASHFOLDER, argv[2]);
                switch (p_error) {
                    case -1:
                        printf("...source file not found!\n");
                        break;
                    case -2:
                        printf("...trash file was not created!\n");
                        break;
                    case -3:
                        printf("...source file could not be removed!\n");
                    default:
                        break;
                }
                break;
            case 'g':
                if (argc != 3) {
                    printf("...not enough arguments!\n");
                    return EXIT_FAILURE;
                }
                int g_error = get_file(TRASHFOLDER, argv[2]);
                switch (g_error) {
                    case -1:
                        printf("...trash file not found!\n");
                        break;
                    case -2:
                        printf("...restore file was not created!\n");
                        break;
                    case -3:
                        printf("...trash file could not be removed!\n");
                    default:
                        break;
                }
                break;
            case 'r':
                if (argc != 3) {
                    printf("...not enough arguments!\n");
                    return EXIT_FAILURE;
                }
                int r_error = remove_file(TRASHFOLDER, argv[2]);
                if (r_error)
                    printf("...trash file could not be removed!\n");
                break;
            default :
                printf("...unknown command!\n");
                return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
