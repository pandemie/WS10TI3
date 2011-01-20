#include <stdlib.h>
#include <stdio.h>
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
#define COPY_SUCCESS        0
#define COPY_FILE_EXISTS    1
#define COPY_NO_SOURCE      2
#define COPY_ERROR          3

char copy_buffer[BUFSIZE];

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
int copyPLUScrc(char *source, char *target);
int copyMINUScrc(char *source, char *target);

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
  //copyPLUScrc("test","./test.crc");
  copyMINUScrc("test.crc","./test");
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

int copyPLUScrc(char *source, char *target)
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
		    write(fd_target,crc,2);
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

int copyMINUScrc(char *source, char *target)
{
    int fd_source, fd_target, tmp;
    int returnval = COPY_ERROR;

    // open source
    fd_source = open(source, O_RDONLY);

    //fseek((FILE *) fd_source, 0, SEEK_END);
    //int sz = ftell((FILE *) fd_source);
    //fseek((FILE *)fd_source, 0, SEEK_SET);

    //printf("%i\n",sz);
    
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
