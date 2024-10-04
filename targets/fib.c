
#include <stdio.h>


long unsigned int fib_L(int n);
long unsigned int fib_R(int n);
int R = 0;
int L = 0;
long unsigned int fib_L(int n){
  L ++;
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;
  return fib_L(n - 1) + fib_R(n - 2);
}

long unsigned int fib_R(int n){
  R ++;
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;
  return fib_L(n - 1) + fib_R(n - 2);
}

int main() { 
  printf("fib(5) = %lu\n", fib_L(5));
  printf("L%d, R%d\n", L, R);

  // printf("fib(7) = %lu\n", fib(8));
  // printf("fib(15) = %lu\n", fib_L(14));
  // printf("fib(19) = %lu\n", fib(19));
}