#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "applib.h"
#include "cJSON.h"
#include "act_debug.h"

#include "common.h"
#include "multicast.h"




struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1500];
static pub_dev_info_t devinfo;

int set_target_id(pub_dev_info_t *devinfo, char *targetid){
    strcpy(devinfo->target_id, targetid);
    return 0;
}

int multisend(char *str, int len){
    int ret;
    struct sockaddr_in groupSock;
    socklen_t sock_len;    
    if(sd < 0){
        perror("sock fd error");
        exit(1);
    }

    bzero(&groupSock, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("224.0.0.1");
    groupSock.sin_port = htons(19602);
    return sendto(sd, str, len, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
}

static int dispatch(char *source, int len){
#if 0
    cJSON *json,*json_name,*json_passwd,*json_num;  
    // char* out="{\"name\":\"fengxin\",\"passwd\":\"123\",\"num\":1}";  
    // char* out = "{\"cmd\":\"hopediscover\",\"params\":{\"deviceid\":\"e076d092265b\"}}";  
    json = cJSON_Parse(source); //解析成json形式  
    json_name = cJSON_GetObjectItem( json , "cmd" );  //获取键值内容    
    json_passwd = cJSON_GetObjectItem( json , "params" );  
    // cJSON *json_param = cJSON_Parse(json_passwd->valuestring);

    json_num = cJSON_GetObjectItem( json_passwd , "deviceid" );  
    printf("name:%s,devid:%s\n",json_name->valuestring,json_num->valuestring);  
    cJSON_Delete(json);  //释放内存   
    return 1;
#else
    int ret = 0;
    cJSON *root = cJSON_Parse(source);
    if(root == NULL){
        return -1;
    }
    cJSON *cmd_item = cJSON_GetObjectItem(root, ITEM_CMD);
    print_zyf("type is %d", cmd_item->type);
    if(cmd_item == NULL){
        printf("get value from cmd error\r\n");
        ret = -1;
        goto Exit;
    }
    if(strcmp(cmd_item->valuestring, CMD_DISCOVER) == 0){
        cJSON *param_item = cJSON_GetObjectItem(root, ITEM_PARAM);
        if(!param_item){
            printf("get params error!");
            ret = -1;
            goto Exit;
        }
        cJSON *device_id_item = cJSON_GetObjectItem(param_item, P_DEV_DEVID);
        if(!device_id_item){
            printf("get dev id error");
            ret = -1;
            goto Exit;
        }
        printf("dev id is %s", device_id_item->valuestring);
        set_target_id(&devinfo, device_id_item->valuestring);
    }
    // cJSON *params_item = cJSON_GetObjectItem(root, "params");
    // if(params_item == NULL){
    //     printf("get value from params error\r\n");
    //     ret = -1;
    //     goto Exit;
    // }
    // cJSON *deviceid = cJSON_GetObjectItem(params_item, "deviceid");
    // if(deviceid == NULL){
    //     printf("get value from deviceid error \r\n");
    //     ret = -1;
    //     goto Exit;
    // }
    // cJSON *bool_item = cJSON_GetObjectItem(params_item, "bool");
    // if(bool_item != NULL){
    //     log("%s type is %d, %s", bool_item->string, bool_item->type, bool_item->valuestring);
    // }
    // cJSON *array_item = cJSON_GetObjectItem(params_item, "arr");
    // log();
    // log("%s, %d, size: %d, %s", array_item->string, array_item->type, cJSON_GetArraySize(array_item), array_item->valuestring);

    // // cJSON *tasklist=array_item->next;//子对象
    // cJSON *tasklist=array_item->child;//子对象
    // while(tasklist!=NULL)
    // {
    //     printf("%d\n",tasklist->valueint);
    //     tasklist=tasklist->next;
    // }
    
    // printf("cmd: %s\r\nparams: %s\r\ndeviceid: %s\r\n", cmd_item->valuestring, params_item->valuestring, deviceid->valuestring);

Exit:
    cJSON_Delete(root);
    return ret;
    #endif
}


int reply(void){
    #if 1
    get_dev_info(&devinfo);
    cJSON *root, *param;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, ITEM_CMD, CMD_DISCOVER);
    cJSON_AddItemToObject(root, ITEM_PARAM, param = cJSON_CreateObject());
    cJSON_AddStringToObject(param, P_DEV_HOPE_ID, devinfo.hope_id);
    cJSON_AddNumberToObject(param, P_DEV_HOPE_PORT, devinfo.hope_port);
    cJSON_AddStringToObject(param, P_DEV_HOPE_IP, devinfo.hope_ip);
    cJSON_AddNumberToObject(param, P_DEV_V_CODE, devinfo.version_code);
    cJSON_AddStringToObject(param, P_DEV_V_NAME, devinfo.version_string);
    cJSON_AddNumberToObject(param, P_DEV_H_VERSION, devinfo.halo_version);
    cJSON_AddNumberToObject(param, P_DEV_PLAYER_TYPE, devinfo.player_type);
    cJSON_AddStringToObject(param, P_DEV_MODEL, devinfo.model);
    cJSON_AddStringToObject(param, P_DEV_TARGET_ID, devinfo.target_id);
    char *out = cJSON_Print(root);
    printf("json create %s\r\n", out);
    cJSON_Delete(root);
    int ret = multisend(out, strlen(out));
    free(out);
    return ret;
    #else
    cJSON *root, *params;
    int ret;
    root = cJSON_CreateObject();   //创建根数据对象  
    params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "hopeid", "12:23:23:34:34:34");
    cJSON_AddNumberToObject(params, "hopeport", 21501);
    cJSON_AddStringToObject(params, "hopeip", "192.168.31.21");
    cJSON_AddNumberToObject(params, "versioncode", 10);
    cJSON_AddStringToObject(params, "versionname", "1.0");
    cJSON_AddNumberToObject(params, "playertype", 1);
    cJSON_AddStringToObject(params, "model", "hope-q3");
    cJSON_AddStringToObject(params, "targetid", "123124214");
    cJSON_AddStringToObject(root, "cmd", "hopediscover");
    cJSON_AddItemReferenceToObject(root, "params", params);
    
    char *out = cJSON_Print(root);
    printf("%s\n",out);  

    ret = multisend(out, strlen(out));

    cJSON_Delete(params);
    cJSON_Delete(root);
    free(out);   
    return ret;
    #endif
}
#if 0
int main(int argc, char *argv[])
{

    int num;

    struct sockaddr_in client;
    socklen_t len;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Opening datagram socket error");
        exit(1);
    } else
        printf("Opening datagram socket....OK.\n");
  
    int opt = SO_REUSEADDR;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(19601);
    localSock.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localSock, sizeof(localSock));

    if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))){
        perror("Binding datagram socket error");
        close(sd);
        exit(1);
    } else
        printf("Binding datagram socket...OK.\n");
  
    // group.imr_multiaddr.s_addr = inet_addr("227.0.0.25");
    // group.imr_interface.s_addr = inet_addr("150.158.231.2");

    group.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    group.imr_interface.s_addr = inet_addr("127.0.0.1");

    // group.imr_interface.S_un.s_addr = inet_addr("127.0.0.1");
    // group.imr_multiaddr.S_un.s_addr = inet_addr("224.0.0.1");

    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    } else
        printf("Adding multicast group...OK.\n");
  
    datalen = sizeof(databuf);
  
    len = sizeof(client);

    while(1){
        printf("wait for multicast\r\n");
        bzero(databuf, sizeof(databuf));
        num = recvfrom(sd, databuf, MAXDATASIZE, 0, (struct sockaddr *)&client,&len);
        if(num < 0){
            close(sd);
            perror("recvfrom error");
            exit(1);
        }
        // databuf[num] = 0;
        printf("recv msg %d, %s\r\n", num, databuf);
        dispatch(databuf, num);
        reply();
        // char *msg = "hello";
        // sendto(sd, msg, strlen(msg), 0, (struct sockaddr*)&client, len);
        // multisend(msg, strlen(msg));
    }
    close(sd);
    return 0;
}
#endif
void multi_cast(){
    int num;
    struct sockaddr_in client;
    socklen_t len;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Opening datagram socket error");
        return;
    } else
        printf("Opening datagram socket....OK.\n");
  
    int opt = SO_REUSEADDR;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(19601);
    localSock.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localSock, sizeof(localSock));

    if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))){
        perror("Binding datagram socket error");
        close(sd);
        return;
    } else
        printf("Binding datagram socket...OK.\n");
  
    // group.imr_multiaddr.s_addr = inet_addr("227.0.0.25");
    // group.imr_interface.s_addr = inet_addr("150.158.231.2");

    group.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    group.imr_interface.s_addr = inet_addr("127.0.0.1");

    // group.imr_interface.S_un.s_addr = inet_addr("127.0.0.1");
    // group.imr_multiaddr.S_un.s_addr = inet_addr("224.0.0.1");

    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    } else
        printf("Adding multicast group...OK.\n");
  
    datalen = sizeof(databuf);
  
    len = sizeof(client);

    while(1){
        printf("wait for multicast\r\n");
        bzero(databuf, sizeof(databuf));
        num = recvfrom(sd, databuf, MAXDATASIZE, 0, (struct sockaddr *)&client,&len);
        if(num < 0){
            close(sd);
            perror("recvfrom error");
            exit(1);
        }
        // databuf[num] = 0;
        printf("recv msg %d, %s\r\n", num, databuf);
        dispatch(databuf, num);
        reply();
        // char *msg = "hello";
        // sendto(sd, msg, strlen(msg), 0, (struct sockaddr*)&client, len);
        // multisend(msg, strlen(msg));
    }
    close(sd);
}


void *halo_udp_proc(void *arg){
    arg = NULL;
    multi_cast();
    pthread_exit(NULL);
}
