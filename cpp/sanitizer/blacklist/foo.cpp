#include <stdlib.h>
void bad_foo() {
  int *a = (int *)malloc(40);
  //   a[9] = 1;
  a[10] = 1;
  free(a);
}

void bad_foo_2() { bad_foo(); }
int main() { bad_foo_2(); }