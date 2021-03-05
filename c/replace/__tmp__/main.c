/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _TAR_   ("##FILE##")
#define _TAR_LEN_   strlen(_TAR_)

char *replace(char *tar, char *arg)
{
    if(!tar) {
        return NULL;
    }
    char *ret = NULL;
    char *ph = tar;
    char *pt = NULL;
    size_t offset = 0;
    size_t arglen = arg ? strlen(arg): 0;
    size_t tarlen = strlen(tar);

    while((pt = strstr(ph, _TAR_)) != NULL) {
        printf("realloc %lu, %lu, %lu\n", tarlen, arglen, tarlen + arglen);
        ret = realloc(ret, (ret ? strlen(ret): 0) + tarlen + arglen);
        memcpy(ret + offset, ph, pt - ph);
        offset += pt - ph;
        memcpy(ret + offset, arg, arglen);
        offset += arglen;
        ret[offset] = 0;
        ph = pt + _TAR_LEN_;
    }
    if(!ret) {
        ret = strdup(tar);
    } else {
        size_t l = strlen(ph);
        memcpy(ret + offset, ph, l);
        *(ret + offset + l) = 0;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    char *ret = replace("echo ##FILE## ##FILE## FILE", "yikexin??");
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("echo", "yke");
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("echo", NULL);
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("echo ##FILE## file", NULL);
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("ls -l", NULL);
    printf("%s\n", ret);
    system(ret);
    free(ret);
    
    ret = replace("ls -l", "ert");
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("ln -sf ##FILE## ##FILE##-1", "inc");
    printf("%s\n", ret);
    system(ret);
    free(ret);

    ret = replace("ln -sf ##FILE## ##FILE##-1", "ILSVRC2012_val_00000015.jpg");
    printf("%s\n", ret);
    free(ret);

    ret = replace("ln -sf ##FILE## ##FILE##-1", "ILSVRC2012_val_00000015.jpg");
    printf("%s\n", ret);
    free(ret);


    return 0;
}
