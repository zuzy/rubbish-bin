#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    #if 1
    FILE *fp = popen("cat > hello.tmp", "w");
    if(fp) {
        char *buf = "hello world\n";
        fwrite(buf, 1, strlen(buf), fp);
        pclose(fp);
    }
    #else
    FILE *fp = popen("cat array0.c", "r");
    if(fp) {
        char buf[100] = "";
        int i = 0;
        while(fgets(buf, sizeof(buf), fp)) {
            printf("%03d %s", ++i, buf);
        }
        printf("\n");
        pclose(fp);
    }
    #endif
    return 0;
}