#include <stdio.h>    
#include <stdlib.h>    
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

static char _num_to_hex(uint8_t num){
    char out = 0;
    num &= 0x0f;
    if(num >= 10){
        out = 'A' + num - 10;
    }else{
        out = '0' + num;
    }
    return out;
}

static int _append_(void **tar, size_t *len, size_t unit)
{
    *len += unit;
    *tar = realloc(*tar, *len);
    return *tar == NULL;
}

char *encode(char *target)
{
    #ifndef JUDGE
    #define JUDGE(d, p, l)    (((p) - (d)) >= (l))
    #endif
    size_t len = 0;
    size_t unit = strlen(target);
    char *ret = NULL;
    size_t offset = 0;
    size_t i = 0;
    char *ptr = ret;
    for(; i < unit; ++i) {
        if(JUDGE(0, offset, len)) {
            if(_append_((void **)&ret, &len, unit)) {
                return ret;
            }
        }
        switch (target[i]) {
            case 'a'...'z':
            case 'A'...'Z':
            case '0'...'9':
            case '.':
            case '-':
            case '_': {
                ret[offset++] = target[i];
                break;
            }
            case ' ': {
                ret[offset++] = '+';
                break;
            }
            default: {
                ret[offset++] = '%';
                if(JUDGE(0, offset, len)){
                    if(_append_((void **)&ret, &len, unit)) {
                        return ret;
                    }
                }
                ret[offset++] = _num_to_hex(target[i] >> 4);
                if(JUDGE(0, offset, len)) {
                    if(_append_((void **)&ret, &len, unit)) {
                        return ret;
                    }
                }
                ret[offset++] = _num_to_hex(target[i]);
                break;
            }
        }
    }
    return ret;
}
