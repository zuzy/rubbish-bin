#include "string.h"
#include "cJSON.h"
#include "halo_common.h"

#define MAX_CMD_NUM     10

// #define CMD_DISCOVER    "hopediscover"
// #define CMD_INFO        "info"
// #define CMD_CONTROL     "control"
// #define CMD_GETSONGLIST "getsonglist"
// #define CMD_GETPLAYLIST "getplaylist"
// #define CMD_QUERYSONINFO    "querysonginfo"
// #define CMD_OPERLIST    "operlist"
// #define CMD_EXIT        "exit"
// #define CMD_OPERTUNNEL  "opertunnel"
// #define CMD_DEVICEINFO  "deviceinfo"

#define cmd_in_range(cmd)    (cmd>=0&&cmd<=MAX_CMD_NUM?1:0)

const char *cmd_str[] = {CMD_DISCOVER, CMD_INFO, CMD_CONTROL, CMD_GETSONGLIST, CMD_GETPLAYLIST, CMD_QUERYSONINFO, CMD_OPERLIST, CMD_EXIT, CMD_OPERTUNNEL, CMD_DEVICEINFO};

int get_cmd(char *cmd){
    int i;
    for(i = 0; i < MAX_CMD_NUM; i++){
        if(strstr(cmd, cmd_str[i])){
            return i;
        }
    }
    return -1;
}

int dispatch_cmd(char *json_string){
    int ret = -1;  
    int cmd_id;  
    cJSON *root = cJSON_Parse(json_string);
    cJSON *cmd_item = cJSON_GetObjectItem(root, ITEM_CMD);
    if(!cmd_item){
        goto Exit;
    }
    cmd_id = get_cmd(cmd_item->valuestring);
    if(cmd_in_range(cmd_id)){
        switch(get_cmd(cmd_item->valuestring)){
            case CID_DISCOVER:{
                printf("multicast cmd!\r\n");
                break;
            }
            case CID_INFO:{
                
                break;
            }
            case CID_CONTROL:{

                break;
            }
            case CID_GETSONGLIST:{

                break;
            }
            case CID_GETPLAYLIST:{

                break;
            }
            case CID_QUERYSONGINFO:{

                break;
            }
            case CID_OPERLIST:{

                break;
            }
            case CID_EXIT:{

                break;
            }
            case CID_OPERTUNNEL:{

                break;
            }
            case CID_DEVICEINFO:{

                break;
            }
            default:{
                break;
            }
        }
    }

Exit:
    cJSON_Delete(root);
}
