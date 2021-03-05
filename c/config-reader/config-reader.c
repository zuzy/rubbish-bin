#include <stdio.h>

#ifdef MAX_LINE_SIZE
#undef MAX_LINE_SIZE
#endif

#define MAX_LINE_SIZE       128
#define _s_MAX_LINE_SIZE    "128"
// #define SCAN_FMT        "%128s %128s"
#define SCAN_FMT        "%" _s_MAX_LINE_SIZE "s %" _s_MAX_LINE_SIZE "s"

int read(char *path, int line, char *fmt, ...)
{
    FILE *f = fopen(path, "r");
    if(!f) {
        return -1;
    }
    int ret = 0;
    int i;
    char c;
    for(i = 0; i < line; ++i) {
        for(;;) {
            c = fgetc(f);
            if(c == '\n') {
                break;
            } else if(c == EOF) {
                ret = -2;
            }
        }
    }
    if(!ret) {
        ret = vfscanf(f, fmt, )
    }

    Ret:
    if(f) {
        fclose(f);
    }
    return ret;
}

void read_config(char *path)
{
    FILE *f = fopen(path, "r");
    if(!f) {
        printf("OPEN %s failed\n", path);
    }
    printf("%s\n", SCAN_FMT);
    int j;
    for(j = 0; ; ++j) {
        char buff[2][MAX_LINE_SIZE] = {{0},{0}};
        int amount = fscanf(f, SCAN_FMT, buff[0], buff[1]);
        int i;
        printf("line: %d\n", j);
        for(i = 0; i < amount; ++i) {
            printf("\t%d -> %s\n", i, buff[i]);
        }
        for(;;) {
            char c = fgetc(f);
            if(c == '\n') break;
            if(c == EOF) return;
        }
    }
}


int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("usage: {path to read}\n");
        return 1;
    }
    read_config(argv[1]);
    return 0;
}