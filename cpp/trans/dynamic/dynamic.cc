#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>

using namespace std;

int FastLog2(int x) {
  float f_x(x);
  unsigned int l_x, exp;

  l_x = *(unsigned int*)&(f_x);
  exp = (l_x >> 23) & 0xFF;
  cout << bitset<32>(x) << endl;
  cout << bitset<32>(f_x) << endl;

//   printf("%s:%d f_x:%f l_x:%u exp:%u\n", __FILE__, __LINE__, f_x, l_x, exp);

  return exp - 127;
}

int main(int argc, char * argv[])
{
    int a = FastLog2(atoi(argv[1]));
    printf("%s:%d return %d\n", __FILE__, __LINE__, a);
    return 0;
}