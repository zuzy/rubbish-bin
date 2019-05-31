#include <string.h>
#include "parse_pipe.h"
#include "buffer.h"

static struct {
    const char *name;
    int (*callback)(void *);
} pipe_handle[] = {
    {"sub"},
    {"pub"},
    {"unsub"},
    {"exit"},
    {"", NULL},
};

int parse_pipe_buf(char *buf, int len)
{
    if(len <= 0 || buf == NULL) {
        return -1;
    }
    char *save = NULL;
    char *tmp = NULL;
    char *delim = ":";
    char *param[10];
    int i = 0;
    
    for(tmp = strtok_r(buf, ":", &save), i = 0; tmp && *tmp; tmp = strtok_r(save, ":", &save), ++i){
        param[i] = tmp;
        if(strstr(param[0], "pub")) {
            if(i >= 2) break;
        }
    }
    print_mqtt("%s: %s",buf, save);
    return 0;
}
