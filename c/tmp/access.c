#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int ret = access("http://tts.dui.ai/runtime/v1/cache/a61e9b7543a8967151914020257ec47f69151a00?productId=278576556", F_OK);
    printf("ret is %d\n", ret);
    return 0;
}