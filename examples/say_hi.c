#include <unistd.h>
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
  while(1){
    sleep(3);
    printf("hi\n");
  }
  get_3();
  printf("bye\n");
  return 0;
}