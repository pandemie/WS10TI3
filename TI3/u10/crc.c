#include <stdlib.h>
#include <stdio.h>

//bit operations on a char array
#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) x[i>>3]|=(1<<(i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

//bit operations on a short
void setbit(short* x, int i);
void clearbit(short* x,int i);
int isbitset(short* x,int i);
void printBinArray(char* ar, int len,int start_bit);
void computeCRC(char* msg,int msg_len, char* poly, int poly_len);

short tmp = 0;
short reg = 0;
char msg[] = {'b',0,0,0,0};
char poly[] = {0,0,0};
int p_len = 17; //crc16
char crc[2] = {0,0};

int main(int argc, char* argv){

  //set the polynom. reverse order because of endianness
  SETBIT(poly,0);//bit 16
  SETBIT(poly,1);//bit 15
  SETBIT(poly,14);//bit 2
  SETBIT(poly,16);//bit 0
  //printBinArray(poly,17);
  //printBinArray(msg,64);

  
  computeCRC(msg,1*8,poly,p_len);
  printBinArray(crc,16,0);
  return 0;
}


void computeCRC(char* msg,int msg_len, char* poly, int poly_len){
  int first_one = 0; //leading one in msg is at msg+first_one
  

  while(first_one < msg_len-1){
    printBinArray(msg,msg_len+poly_len,0);
    //move the message to match the first 1
    while(!ISBITSET(msg,first_one)){
      ++first_one;
    }
    printf("%i\n",first_one);
    printBinArray(msg,poly_len,first_one);
    printBinArray(poly,poly_len,0);

    //division
    for(int i = 0;i<poly_len;++i){
      if(ISBITSET(msg,first_one+i) != ISBITSET(poly,i)){
	SETBIT(msg,first_one+i);
      }else{
	CLEARBIT(msg,first_one+i);
      }
    }
    printBinArray(msg,poly_len,first_one);
    printBinArray(msg,msg_len+poly_len,0);


    //break;
  }
  
  for(int i = 0; i<poly_len-1; ++i){
    ++first_one;
    if(ISBITSET(msg,first_one)){
      SETBIT(crc,i); 
    }else{
      CLEARBIT(crc,i);
    }
  }
}

void printBinArray(char* ar, int len, int start_bit){
  for(int i = start_bit; i<len+start_bit; ++i){
    if(ISBITSET(ar,i)){
      printf("%i",1);
    }else{
      printf("%i",0);
    }
  }
  printf("\n");
}


void setbit(short* x,int i){
  *x|=(1<<i);
}

void clearbit(short* x,int i){
  *x&=(~(1<<i));
}

int isbitset(short* x,int i){
  return (*x|(1<<i))==*x;
}
