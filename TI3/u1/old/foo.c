#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>

int str2int(char*);
int char2int(char);
int power(int,int);

int main(void){
  //char* word = "11231\n";
  //printf("Hello %i World!\n",str2int(word));
  char buffer[20];
  size_t readRet;
  int firstNumber;
  FILE* myFile;

  myFile = fopen("test.dat","rw");
  
  readRet = fread(buffer,8,20,myFile);
  firstNumber = atoi(buffer);
 
  do{
    
/*   } while ((readRet = fread(buffer,8,20,myFile)) > 0) */
 
/* } */

int str2int(char* ptr){
  char* oriPtr = ptr;
  int result = 0;

  while(char2int(*(ptr+1)) != -1){
    ++ptr;
  }
  for(int i = 0;ptr >= oriPtr;++i){
    result += char2int(*(ptr--))*power(10, i);
  }
  return result;
}
  
int char2int(char ch){
  int result = ch-'0';
  if(result >= 0 && result <= 9){
    return result;
  } else {
    return -1;
  }
}

int power(int a, int b){
  if(b == 0){
    return 1;
  }
  int result = a;
  for(int i = 1; i < b; ++i){
    result = result * a; 
  }
  return result;















}
