#include <stdio.h>
#include <stdlib.h>
#include <string.h>


short getSectorSize(FILE * myFile);
char  getClusterSize(FILE * myFile);
short getReservedSize(FILE * myFile);
char  getFATcount(FILE * myFile);
short getRootSize(FILE * myFile);
int   getFSsize(FILE * myFile);
short getFATsize(FILE * myFile);
int   getFileStartSector(int x, FILE * myFile);
int   getRootSector(FILE * myFile);
void  printDir(char* path, FILE * myFile);
int   getClusterAdress(int number, FILE * f);

//returns a clusternumber int the startsector of that cluster
int getFileStartSector(int x, FILE * f){
  int r;
  r = getReservedSize(f)+getFATcount(f)*getFATsize(f)
    + div((getRootSize(f)*32),getSectorSize(f)).quot
    +(x-2)*getClusterSize(f);
  return r;
}

//returns the start sector of the root directory
int getRootSector(FILE * f){
  return getReservedSize(f)+getFATcount(f)*getFATsize(f);
}

int getClusterAdress(int number, FILE * f){
  return (getReservedSize(f)+getFATcount(f)*getFATsize(f) + getRootSize(f)*32/getSectorSize(f)
	  + (number - 2)*getClusterSize(f)) * getSectorSize(f);
}

//returns the number of bytes in a sector
short getSectorSize(FILE * myFile){
  short s;
  fseek(myFile, 11, SEEK_SET);
  fread(&s,2,1,myFile);
  return s;
}

//returns the number  of sectors in a cluster
char getClusterSize(FILE * myFile){
  char c;
  fseek(myFile, 13, SEEK_SET);
  fread(&c,1,1,myFile);
  return c;
}

//number of reserved sectors
short getReservedSize(FILE * myFile){
  short s;
  fseek(myFile,14, SEEK_SET);
  fread(&s,2,1,myFile);
  return s;
}

//nuber of FATables
char  getFATcount(FILE * myFile){
  char c;
  fseek(myFile, 16, SEEK_SET);
  fread(&c,1,1,myFile);
  return c;
}

//returns maximum number of files in root directory
short  getRootSize(FILE * myFile){
  short s;
  fseek(myFile, 17, SEEK_SET);
  fread(&s,2,1,myFile);
  return s;
}

//returns the size of the filesystem in sectors
int getFSsize(FILE * myFile){
  int i = 0;
  fseek(myFile, 19, SEEK_SET);
  fread(&i,2,1,myFile);
  if(0 == i){
    fseek(myFile, 32, SEEK_SET);
    fread(&i,4,1,myFile);
  }
  return i;
}

//size of a FAT in sectors
short getFATsize(FILE * myFile){
  short s;
  fseek(myFile, 22, SEEK_SET);
  fread(&s,2,1,myFile);
  return s;
}
