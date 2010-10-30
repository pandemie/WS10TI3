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
 
  size_t buffer_size = 128;
  char buffer[buffer_size];
  
  fd_source = open(source, O_RDONLY);
  if(fd_source < 0){
    return -1;
  }
  fd_target = open(target, O_APPEND | O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);

  while(read(fd_source, buffer, buffer_size) != 0){
    write(fd_target, buffer, buffer_size);
  }
  
  close(fd_target);
  close(fd_source);
}


char parse_command(char *command)
{
	return command[1];
}


/* erzeugt einen Ordner foldername */
int setup_trashcan(char *foldername)
{
  return (int) mkdir(foldername);
}

/* führt trashcan -p[ut] filename aus */
int put_file(char *foldername, char *filename)
{
  char str[512];
  char* str_copy = str;

  while(*foldername != 0){
    *(str_copy++) = *(foldername++);
  }
  *(str_copy++) = '/';
  while(*filename != 0){
    *(str_copy++) = *(filename++);
  }
  *(str_copy++) = 0;

  printf("%i\n",setup_trashcan(foldername));
  copy(filename, str);
  
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
  char source[] ="trashcan_framework.c";
  char target[] = "test_can";
  put_file(source, target);
  
  /*
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
  */
	return EXIT_SUCCESS;
}
