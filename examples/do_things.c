#include <stdio.h>
#include <unistd.h>

void do_thing(){  
  printf("doing thing...\n");
  sleep(3);
  printf("done!\n");
  int i;
  for (i = 0; i < __INT_MAX__; i ++){

  }
  printf("%lx\n", i);
}

int main() { 
  while(1){
    do_thing();
  }
}
