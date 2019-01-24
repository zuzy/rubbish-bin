
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcp_server.h"
#include "cJSON.h"

#define EXIT_CONNECTION (-10)

static int cmd_info(void *ptr);
static int cmd_control(void *ptr);
static int cmd_getsonglist(void *ptr);
static int cmd_getplaylist(void *ptr);
static int cmd_querysonginfo(void *ptr);
static int cmd_operlist(void *ptr);
static int cmd_exit(void *ptr);
static int cmd_opertunnel(void *ptr);
static int cmd_deviceinfo(void *ptr);
static int cmd_hopediscover(void *ptr);

typedef int (*getparams)(cJSON*);

#define CREATE_CMD(str) {#str,(getparams)cmd_##str}

#define CREATE_PARAM(str,i) {str,i}

#if 0
#define SEARCH_PARAM(param,json) cJSON_GetObjectItem(json,#param)?param:NULL
#else
#define SEARCH_PARAM(param,json,out) do{if(cJSON_GetObjectItem(json,#param)){ \
                                struct _st_ctrl_param *ptr=param; \
                                while(ptr->cmd){if(!(strcmp(ptr->cmd,cJSON_GetObjectItem(json,#param)->valuestring)))break; ++ptr;}\
                                out=ptr->index;}}while(0)

#define GET_PARAM(param,json,op)    SEARCH_PARAM(param,json,op.param);
#endif


static struct _st_cmd{
    char *cmd;
    getparams callback;
}_st_cmd;

// int dispatch_server(char *recvbuf, int num);

const struct _st_cmd cmd_list[] = {
    CREATE_CMD(info),
    CREATE_CMD(control),
    CREATE_CMD(getsonglist),
    CREATE_CMD(getplaylist),
    CREATE_CMD(querysonginfo),
    CREATE_CMD(operlist),
    CREATE_CMD(exit),
    CREATE_CMD(opertunnel),
    CREATE_CMD(deviceinfo),
    CREATE_CMD(hopediscover),
    {NULL, NULL}
};





struct _st_ctrl_param{
    char *cmd;
    int index;
};

struct _st_operation{
    int effect;
    int source;
    int mode;
    int playstate;
    int volume;
};

// int dispatch_param(struct _st_ctrl_param *list, char *cmd){
//     if()
// }

const struct _st_ctrl_param effect[] = {
    CREATE_PARAM("classical", 0),
    CREATE_PARAM("modern", 1),
    CREATE_PARAM("rockroll", 2),
    CREATE_PARAM("pop", 3),
    CREATE_PARAM("dance", 4),
    CREATE_PARAM("original", 5),
    {NULL, -1}
};

const struct _st_ctrl_param source[] = {
    CREATE_PARAM("local", 0),
    CREATE_PARAM("bluetooth", 1),
    CREATE_PARAM("linein", 2),
    {NULL,-1}
};

const struct _st_ctrl_param mode[] = {
    CREATE_PARAM("random", 0),
    CREATE_PARAM("single", 1),
    CREATE_PARAM("cycle", 2),
    CREATE_PARAM("list", 4),
    {NULL, -1}
};

const struct _st_ctrl_param playstate[] = {
    CREATE_PARAM("play", 0),
    CREATE_PARAM("pause", 1),
    CREATE_PARAM("prev", 2),
    CREATE_PARAM("next", 3),
    {NULL,-1}
};

const struct _st_ctrl_param volume[] = {
    CREATE_PARAM("inc",0x40),
    CREATE_PARAM("dec",0x41),
    CREATE_PARAM("mute",0x42),
    CREATE_PARAM("demute",0x43),
    {NULL,-1}
};

static int _parse_volume(cJSON *json){
    int ret;    
    cJSON *vol_item = cJSON_GetObjectItem(json, "volume");
    if(!vol_item){
        return -1;
    }
    char vol_str[4] = {0};
    int len = strspn(vol_item->valuestring, "0123456789");
    if(len){
        strncpy(vol_str, vol_item->valuestring, len);
        ret = atoi(vol_str);
    }else{
        SEARCH_PARAM(volume, json, ret);
    }
    return ret;
}


static void hex_dump(char *str, size_t len){
    size_t i;
    printf("get hex ");
    for(i = 0; i < len; i++){
        printf(" %02x", str[i]);
    }
    printf("\n");
}

static int cmd_hopediscover(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_info(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_control(void *input){
    if(!input){
        return -1;
    }
    struct _st_operation operation = {-1,-1,-1,-1};
    printf("cmd ctrl\n");
    cJSON *json = (cJSON *)input;
    printf("%s\n", cJSON_Print(json));
    // int ret = -10;
    // SEARCH_PARAM(effect,json,ret);
    GET_PARAM(effect, json, operation);
    GET_PARAM(mode, json, operation);
    GET_PARAM(source, json, operation);
    GET_PARAM(playstate, json, operation);
    operation.volume = _parse_volume(json);
    // SEARCH_PARAM(playstate,json,operation.playstate);

    // SEARCH_PARAM(volume,json,operation.volume);

    printf("effect: %d\n", operation.effect);
    printf("model: %d\n", operation.mode);
    printf("source: %d\n", operation.source);
    printf("playstate: %d\n", operation.playstate);
    printf("volume: %d\n", operation.volume);
    return 0;
}
static int cmd_getsonglist(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_getplaylist(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_querysonginfo(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_operlist(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_exit(void *input){
    return EXIT_CONNECTION;
    if(!input){
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_opertunnel(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}
static int cmd_deviceinfo(void *input){
    if(!input){
        return -1;
    }
    cJSON *json = (cJSON *)input;
}

