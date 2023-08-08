#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>

using namespace std;


int main(int argc, char * argv[])
{
    uint8_t a = 0xff;
    int8_t b = static_cast<int8_t>(a);
    printf("a:%d, b:%d\n", a, b);
    std::cout << "a: " << a << " b: " << b << std::endl;
    return 0;
}