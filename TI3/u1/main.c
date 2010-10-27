#include <stdio.h>


int main(int argc, char **argv){
  FILE *file;
  int target;
  int scancode;
  int sum = 0;
  int a, b;

  //check for enough arguments
  if(argc < 2){
    printf("please pass filename as argumet\n");
    return 1;
  }

  //Open file and check for success.
  file = fopen(argv[1],"r");
  if(file == 0){
    printf("Failure reading file\n");
    return 1;
  }
  
  //Read first line and check
  if(fscanf(file,"%i",&target) != 1){
    printf("Failure reading first line\n");
    return 1;
  }

  //compute the sum
  while((scancode = fscanf(file,"%i*%i",&a,&b)) == 2){
    sum = sum + a*b;
  }

  //check for the last return value of fscanf() and print adequate message
  if(scancode == EOF){
    printf("The target was %i.\n",target);
    printf("The calculated sum was: %i\n",sum);
    if(sum == target){
      printf("traget equals result\n");
    } else {
      printf("target not met\n");
    }
  } else {
    printf("wrong syntax\n");
    return 1;
  }
  
  return 0;
}
