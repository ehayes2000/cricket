
#include <stdio.h>


long unsigned int fib_L(int n);
long unsigned int fib_R(int n);

long unsigned int fib_L(int n){
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;
  return fib_L(n - 1) + fib_R(n - 2);
}

long unsigned int fib_R(int n){
  if (n == 0)
    return 0;
  if (n == 1)
    return 1;
  return fib_L(n - 1) + fib_R(n - 2);
}

int main() { 
  // printf("fib(5) = %lu\n", fib(5));

  // printf("fib(7) = %lu\n", fib(8));
  printf("fib(15) = %lu\n", fib_L(14));
  // printf("fib(19) = %lu\n", fib(19));
}