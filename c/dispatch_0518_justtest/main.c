#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "cJSON.h"


typedef struct{
    int status;
    int play;
    int control;
    long skip;
    int idvol;
    int mute;
    int setvol;
    int source;
    int effect;
    int model;
    int locale;
}hope_ctrl_t;


static void _add(cJSON *json, int num, char *key){
    if(num >= 0){
        char *p = NULL;
        if((p = strrchr(key, '.')) == NULL){
            p = key;
        }else{
            ++p;
        }
        cJSON_AddNumberToObject(json, p, num);
    }
}

#define ADD_TO_JSON(json,ctrl) _add(json,ctrl,#ctrl)

int _add_to(cJSON *json, int n, char *str){
    if(n >= 0){
        char *p = NULL;
        if((p = strrchr(str, '.')) != NULL){

        }
        cJSON_AddNumberToObject(json, (const char*)str, n);
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[]){
    #define _GET_STR(A) #A
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "root", "hello");
    hope_ctrl_t ctrl = {1,1,1,1,1,1,1,1,1};

    char *str = malloc(20);
    char *p;
    int asdf = 10;
    strcpy(str, _GET_STR(ctrl.status));
    p = strrchr(str, '.');
    p++;
    _add_to(root, ctrl.status, p);

    ADD_TO_JSON(root,ctrl.play);
    ADD_TO_JSON(root,ctrl.skip);
    ADD_TO_JSON(root,ctrl.status);
    ADD_TO_JSON(root, asdf);

    char *out = cJSON_Print(root);
    
    printf("%s\n", out);
    return 0;
}