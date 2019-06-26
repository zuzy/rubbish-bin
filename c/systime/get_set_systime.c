#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_time()
{
    char buf[1024] = {0};
    FILE *f = popen("date", "r");
    int nread = fread(buf, 1, sizeof(buf), f);
    if(nread > 0) {
        printf("get system %s\n", buf);
    } else {
        printf("get time failed\n");
    }
    pclose(f);
}

int main(int argc, char *argv[])
{
    print_time();
    srand(time(NULL));
//    time_t t = (time_t)random();
    time_t t = rand();
    printf("random %ld\n", t);
    stime(&t);
    time(&t);
    t = -1;
    
    printf("time is %ld\n", t);
    print_time();
    return 0;
}
