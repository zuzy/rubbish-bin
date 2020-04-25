#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PAYLOAD_HEAD       "[HEAD]"
#define HEAD_OFFSET         (strlen(PAYLOAD_HEAD))
#define SIZE_OFFSET         (sizeof(PAYLOAD_HEAD))

int main(int argc, char*argv[])
{
    printf("%s : %d %d\n", PAYLOAD_HEAD, (int)HEAD_OFFSET, (int)SIZE_OFFSET);
    return 0;
}