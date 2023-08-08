#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>

using namespace std;
// #define use_union

int FastLog2(int x) {
  unsigned int l_x, exp;
#ifndef use_union
  float f_x(x);
  l_x = *(unsigned int*)&(f_x);
#else
  union {
    float f;
    uint32_t u32;
  } f_x;
  f_x.f = x;
  l_x = f_x.u32;
#endif
  exp = (l_x >> 23) & 0xFF;
  cout << bitset<32>(x) << endl;
#ifndef use_union
  cout << bitset<32>(f_x) << endl;
#else
  cout << bitset<32>(f_x.f) << " && " << bitset<32>(f_x.u32) << endl;
#endif

//   printf("%s:%d f_x:%f l_x:%u exp:%u\n", __FILE__, __LINE__, f_x, l_x, exp);

  return exp - 127;
}

int main(int argc, char * argv[])
{
    int a = FastLog2(atoi(argv[1]));
    printf("%s:%d return %d\n", __FILE__, __LINE__, a);
    return 0;
}