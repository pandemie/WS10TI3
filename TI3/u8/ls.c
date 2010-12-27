#include <stdio.h>

short getSectorSize(FILE * myFile);
char  getClusterSize(FILE * myFile);
short getReservedSize(FILE * myFile);
char  getFATcount(FILE * myFile);
short getRootSize(FILE * myFile);
int   getFSsize(FILE * myFile);
short getFATsize(FILE * myFile);

int main(int argc, char* argv){
  
  FILE * myFile = fopen("drive.img","r");
  printf("%d\n",getSectorSize(myFile));
  printf("%d\n",getClusterSize(myFile));
  printf("%d\n",getReservedSize(myFile));
  printf("%d\n",getFATcount(myFile));
  printf("%d\n",getRootSize(myFile));
  printf("%d\n",getFSsize(myFile));
  printf("%d\n",getFATsize(myFile));
  return 0;
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
