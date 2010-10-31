/* trashcan.c */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE			1024
#define PATHSIZE		256
#define TRASHFOLDER	".ti3_trash"	


char copy_buffer[BUFSIZE];



/*
/* 'copy' kopiert den Dateiinhalt einer Datei namens 
 * "source". Eine Kopie target wird nur erzeugt, wenn 
 * eine Datei "target" noch nicht existiert.
 * Die Zugriffsrechte werden nicht kopiert, sondern
 * auf "rwx --- ---" gesetzt.
 */
int copy(char *source, char *target)
{
  int fd_source;
  int fd_target;
 
  fd_source = open(source, O_RDONLY);
  if(fd_source < 0){
    return -1;
  }
  fd_target = open(target, O_APPEND | O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);

  while(read(fd_source, copy_buffer, BUFSIZE) != 0){
    write(fd_target, copy_buffer, BUFSIZE);
  }
  
  close(fd_target);
  close(fd_source);
  return 1;
}


char parse_command(char *command)
{
	return command[1];
}


/* erzeugt einen Ordner foldername */
int setup_trashcan(char *foldername)
{
  int success = (int) mkdir(foldername,0777);
  
  int er = errno;
  printf("%s\n", strerror( er ));
  return success;
}

/* führt trashcan -p[ut] filename aus */
int put_file(char *foldername, char *filename)
{
  char path[512];
  char* path_copy = path;
  char* foldername_copy = foldername;
  char* filename_copy = filename;

  while(*foldername_copy != 0){
    *(path_copy++) = *(foldername_copy++);
  }
  *(path_copy++) = '/';
  while(*filename_copy != 0){
    *(path_copy++) = *(filename_copy++);
  }
  *(path_copy++) = 0;

  copy(filename, path);
  int er = errno;
  if(er == 17 || er == 9)
    printf("%s\n",strerror(er));
  return 1;
}


/* führt trashcan -g[et] filename aus */
int get_file(char *foldername, char *filename)
{

}

/* führt trashcan -r[emove] filename aus */
int remove_file(char *foldername, char *filename)
{

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
