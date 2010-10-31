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

int remove_file(char*,char*);


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
  return 0;
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
  char path[512];
  merge_path(foldername, filename, path);
  if(copy(filename, path) == -1){
    return -1;
  }

  if(errno == 9){
    printf("file allready exist\n");
    return -2;
  }

  char dot[] = ".";
  if (remove_file(dot, filename)  == -1){
    return -3;
  }

  return 0;
}


/* führt trashcan -g[et] filename aus */
int get_file(char *foldername, char *filename)
{
  char path1[512];
  char path2[512];
  char dot[] = ".";
  merge_path(foldername,filename,path1);
  merge_path(dot,filename,path2);
  copy(path1,path2);
  remove_file(dot,path1);
}

/* führt trashcan -r[emove] filename aus */
int remove_file(char *foldername, char *filename)
{
  char path[512];
  merge_path(foldername, filename, path);
  return unlink(path);
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
