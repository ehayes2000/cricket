#include <stdio.h>

int get_3(){
  int four = 4;
  int one = 1;
  return four - one;
}

int main(){  
  printf("hello world\n");
  int three = get_3();
  printf("4 - 1 is %d\n", three);
  return 0;
}