#include <time.h>
#include <string.h>

#define _UNKNOW_TIME    "<Unknow-time>"

inline int formatNowTime(char *tar, size_t size)
{
    time_t _tm = time(NULL);
    struct tm *tmp = localtime(&_tm);
    if(tmp == NULL || strftime(tar, size, "<%F %H:%M:%S>", tmp) == 0) {
        strcpy(tar, "<Unknow-time>");
        return 1;
    }
    return 0;
}