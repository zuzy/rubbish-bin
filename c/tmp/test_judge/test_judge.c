#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

int main()
{
    int s = 32 * 1024;
    int d = s >> 15? : 2;
    printf("%d \n", d);
    char tar[25];
    formatNowTime(tar, sizeof(tar));
    printf("%s \n", tar);
    printf(_UNKNOW_TIME"\n");



    time_t _tm = time(NULL);
    struct tm *tmp = localtime(&_tm);
    char outstr[25] = "";
    if (tmp != NULL) {
        if (strftime(outstr, sizeof(outstr), "<%F %H:%M:%S>", tmp) == 0) {
            strcpy(outstr, "<Unknow-time>");
        }
    }

    printf("%s\n", outstr);
    return 0;
}