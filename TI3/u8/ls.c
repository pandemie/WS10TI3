#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

#define MAX_FOLDER_DEPTH 64

//short getSectorSize(FILE * myFile);
//char  getClusterSize(FILE * myFile);
//short getReservedSize(FILE * myFile);
//char  getFATcount(FILE * myFile);
//short getRootSize(FILE * myFile);
//int   getFSsize(FILE * myFile);
//short getFATsize(FILE * myFile);
//int   getFileStartSector(int x, FILE * myFile);
//int   getRootSector(FILE * myFile);
//void  printDir(char* path, FILE * myFile);
//int getClusterAdress(int number, FILE * f);
void printDir(char* path, FILE * f);
void printDirAt(int adress, FILE * f);
int  getFolderAdress(char* path, FILE * f);

int main(int argc, char* argv){
  
  FILE * myFile = fopen("drive.img","r");
  printf("sector size: %d\n",getSectorSize(myFile));
  printf("sectors per cluster: %d\n",getClusterSize(myFile));
  printf("reserved sectors: %d\n",getReservedSize(myFile));
  printf("FATcount: %d\n",getFATcount(myFile));
  printf("RootSize: %d\n",getRootSize(myFile));
  printf("FSsize in sectors: %d\n",getFSsize(myFile));
  printf("FATsize: %d\n",getFATsize(myFile));
  printf("3. Cluster: %d\n",getFileStartSector(3,myFile));
  printf("Adress of 3. Cluster: %x\n",getFileStartSector(0,myFile)*getSectorSize(myFile));
  printf("Sector of root dir: %x\n",getRootSector(myFile));
  printf("adress of Sector of root dir: %x\n",getRootSector(myFile)*getSectorSize(myFile));
  //getFolderAdress("/IF/HALLO",myFile);
  //printDir("/",myFile);
  int b = 3;
  printf("cluster %i adress: %x\n",b,getClusterAdress(b,myFile));
  printDirAt(getClusterAdress(b,myFile),myFile);

  return 0;
}

//prints the contens of a directory
void printDir(char* path, FILE * f){
  if(strcmp(path,"/") == 0){
    int adress = getRootSector(f)*getSectorSize(f);
    printDirAt(adress,f);
  } else {
    char* dirs[11*MAX_FOLDER_DEPTH];
    
  }
  
}

int getFolderAdress(char* path, FILE * f ){
  if(*path != '/'){
    printf("no slash\n");
  }
  ++path;
  char* current = path;
  char type;
  char c;
  int adress = getRootSector(f)*getSectorSize(f);
  fseek(f,adress+11, SEEK_SET);
  fread(&type,1,1,f);
    
  while(type != 0){
    //directory
    if(type == 0x10 && checkDir(path, adress)){
      adress = adress+26;
      fseek(f,adress+11, SEEK_SET);
      fread(&type,1,1,f);
      while(0){
	if(*path == '/'){
	  ++path;
	  break;
	}else{
	  ++path;
	}
      }
    } else {
      adress = adress+0x20;
      fseek(f,adress+11, SEEK_SET);
      fread(&type,1,1,f);
    }
  }    
}



void printDirAt(int adress, FILE * f){
  
  char type;
  char c;
  fseek(f,adress+11, SEEK_SET);
  fread(&type,1,1,f);
    
  while(type != 0){

    //directory
    if(type == 0x10){
      for(int i = 0; i<11;++i){
	fseek(f, adress+i, SEEK_SET);
	fread(&c,1,1,f);
	  
	if(c == ' '){
	  break;
	} else {
	  printf("%c",c);
	}
	  
      }
      printf("<dir>\n");
    }
    adress = adress+0x20;
    fseek(f,adress+11, SEEK_SET);
    fread(&type,1,1,f);      
  }    
}

int checkDir(char* path, char* dirname){
  for(int i = 0;i<11;++i){
    if((*path == '/' || *path == 0) && (*dirname == ' ' || *dirname == 0x10)){
      return 0;
    } else if(*path != *dirname){
      return 1;
    }
    ++path;
    ++dirname;
  }
}
