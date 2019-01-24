#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

#include "applib.h"
#include "act_debug.h"

#include "net/config.h"
#include "net/wifi_manager.h"

#include "cloud_client.h"
#include "cJSON.h"
#include "command.h"
#include "common.h"
#include "http_regist.h"
#include "cloud_client.h"

static int sockfd = -1;
static char *buf[1024];
static int num;
static volatile int recv_status;
static cloud_head_t local_head;
static pthread_t client_tid;
pthread_mutex_t client_lock;
static char auth_code[33];
// static unsigned char ref_id[11];

#define GET_INT(json,a) ((json)?((json)->valueint):(a))
#define GET_INT_DEFAULT(json)   GET_INT(json,-1)
#define GET_BOOL(json) ((json)?((json)->type):-1)

static int identify();
static int heart_beat(void *arg);
static int dispatch_command(char *buf, int len);

static void *client_proc(void *arg){
    print_zyf("hope client start");
    arg = NULL;
    int timer_id;
    fd_set read_fds;
    struct timeval t;
    struct sockaddr_in server;
    int sel_ret;
    wifi_info_t wifi_info;
    pthread_mutex_init(&client_lock, NULL);
    while(1){

        // wifi_get_wifi_info(&wifi_info);
        // while(wifi_info.link_speed <= 0){
        //     sleep(1);
        // }
        print_zyf("cloud client start");
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
            print_zyf("creat socket error");
            goto Exit;
        }
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(CLOUD_HOST_PORT);
        server.sin_addr.s_addr = inet_addr(CLOUD_HOST_NAME);
        if(connect(sockfd, (struct in_addr*)&server, sizeof(server)) == -1){
            print_zyf("connect failed");
            goto Exit;
        }
        identify();
        // print_zyf("sock connected start timer");
        timer_id = set_timer(30000, heart_beat, NULL);
        // heart_beat(NULL);
        while(1){
            t.tv_sec = 30;
            t.tv_usec = 0;
            FD_ZERO(&read_fds);
            FD_SET(sockfd, &read_fds);
            sel_ret = select(sockfd + 1, &read_fds, NULL, NULL, &t);
            if(sel_ret > 0){
                if(FD_ISSET(sockfd, &read_fds)){
                    num = recv(sockfd, buf, 1024, 0);
                    print_zyf("recv %d", num);
                    if(num <= 0){
                        print_zyf("recv error");
                        break;
                    }else{
                        dispatch_command(buf, num);
                    }
                }
            }else if(sel_ret == 0){
                heart_beat(NULL);
            }else if(sel_ret == EINTR){
                continue;
            }else{

            }
            usleep(50);
        }
    }
Exit:
    kill_timer(timer_id);
    pthread_mutex_destroy(&client_lock);
    close(sockfd);
    sockfd = -1;
}

int cloud_client_start(){
    int ret = -1;
    bzero(auth_code, 33);
    bzero(local_head.snd_id, 10);
    if(regist_device(auth_code, local_head.snd_id) == 200){
        regist_property();
        print_zyf("regist ok %s", auth_code);
        ret = pthread_create(&client_tid, NULL, client_proc, NULL);
    }else{
        print_zyf("regist error");
    }
    return ret;
}

int cloud_client_close(){
    if(client_tid >= 0)
        return pthread_join(client_tid, NULL);
    else
        return -1;
}

// body is json no need to 7e 7d
#if 0
static int _send(cloud_head_t head, char *body){
    if(sockfd < 0){
        return -1;
    }
    char *head_ptr = &head;
    char *send_buf = (char*)malloc(256);
    if(send_buf == NULL){
        return -1;
    }
    char *send_ptr = send_buf;
    int i, size, ret;
    uint8_t hex_checkout;
    bzero(send_buf, 256);
    *send_ptr++ = 0x7e;
    size = sizeof(head);
    for(i = 0; i < size; i++){
        if(0 == i){
            hex_checkout = *head_ptr;
        }else{
            hex_checkout ^= *head_ptr;
        }
        if(*head_ptr != 0x7e && *head_ptr != 0x7d){
            *send_ptr++ = *head_ptr++;
        }else if(*head_ptr == 0x7e){
            *send_ptr++ = 0x7d;
            *send_ptr++ = 0x02;
            ++head_ptr;
        }else{
            *send_ptr++ = 0x7d;
            *send_ptr++ = 0x01;
            ++head_ptr;
        }
    }
    memcpy(send_ptr, body, strlen(body));
    send_ptr+=strlen(body);
    if(hex_checkout != 0x7e && hex_checkout != 0x7d){
        *send_ptr = hex_checkout;
    }else if(hex_checkout == 0x7e){
        *send_ptr++ = 0x7d;
        *send_ptr = 0x02;
    }else{
        *send_ptr = 0x7d;
        *send_ptr = 0x01;
    }
    size = send_ptr - send_buf;
    ret = send(sockfd, send_buf, size, 0);
    free(send_buf);
    return ret;
}
#else
static int _send(cloud_head_t head, char *body){
    print_zyf("head:%d,%d,%d, %s", head.cmd, head.type, head.len, body);
    if(sockfd < 0){
        return -1;
    }
    char *head_ptr = &head;
    char *send_buf = (char*)malloc(256);
    if(send_buf == NULL){
        return -1;
    }
    char *send_ptr = send_buf;
    int i, size, ret;
    uint8_t hex_checkout;
    bzero(send_buf, 256);
    *send_ptr++ = 0x7e;
    size = sizeof(head);
    for(i = 0; i < size; i++){
        if(0 == i){
            hex_checkout = *head_ptr;
        }else{
            hex_checkout ^= *head_ptr;
        }
        if(*head_ptr != 0x7e && *head_ptr != 0x7d){
            *send_ptr++ = *head_ptr++;
        }else if(*head_ptr == 0x7e){
            *send_ptr++ = 0x7d;
            *send_ptr++ = 0x02;
            ++head_ptr;
        }else{
            *send_ptr++ = 0x7d;
            *send_ptr++ = 0x01;
            ++head_ptr;
        }
    }
    #if 0
    memcpy(send_ptr, body, strlen(body));
    send_ptr+=strlen(body);
    #else
    size = strlen(body);
    for(i = 0; i < size; i++){
        *send_ptr++ = body[i];
        hex_checkout ^= body[i];
    }
    #endif
    printf("checkout is %02x\r\n", hex_checkout);
    if(hex_checkout != 0x7e && hex_checkout != 0x7d){
        *send_ptr++ = hex_checkout;
    }else if(hex_checkout == 0x7e){
        *send_ptr++ = 0x7d;
        *send_ptr++ = 0x02;
    }else{
        *send_ptr++ = 0x7d;
        *send_ptr++ = 0x01;
    }
    *send_ptr = 0x7e;
    // printf("checkout is %02x\r\n", *send_ptr);
    size = send_ptr - send_buf + 1;
    
    
    ret = send(sockfd, send_buf, size, 0);
    printf("send buf %d:", size);
    int k = 0;
    for(k = 0; k < size; k++){
        printf(" %02x", send_buf[k]);
    }
    printf("\r\n");
    free(send_buf);
    return ret;
}

#endif
static int cloud_send(uint16_t cmd, char *input){    
    local_head.cmd = htons(cmd);
    local_head.type = 1;
    local_head.len = htons(strlen(input) << 1);
    return _send(local_head, input);
}

static int identify(){
    cJSON *root = cJSON_CreateObject();
    if(!root){
        return -1;
    }
    cJSON_AddStringToObject(root, "code", auth_code);
    int ret = cloud_send(C_DEV_IDENTIFY, cJSON_Print(root));
    cJSON_Delete(root);
    return ret;
}

// static int identify(){
//     int ret;
//     cJSON *iden = cJSON_CreateObject();
//     if(!iden){
//         return -1;
//     }
//     cJSON_AddStringToObject(iden, "code", auth_code);
//     ret = cloud_send(C_DEV_IDENTIFY, cJSON_Print(iden));
//     cJSON_Delete(iden);
//     return ret;
// }

static int heart_beat(void *arg){
    print_zyf("heatrbeat");
    return cloud_send(C_CMD_HEART, "");
}
/*
    get head and body;
    with ori format head(small encode);
*/

static int dispatch_buf(int len, char *buf, cloud_head_t *head, char *body){
    unsigned char *buf_ptr = buf;
    char *body_ptr = body;
    char *head_ptr = head;
    int i, size;
    uint8_t checksum;
    uint8_t hex_check = 0;
    if(len < 18){
        print_zyf("buff is too short");
        return -1;
    }
    if(*buf_ptr != 0x7e){
        print_zyf("mark bit error");
        return -1;
    }
    buf_ptr++;
    size = sizeof(cloud_head_t);
    for(i = 0; i < size; i++){
        if(*buf_ptr == 0x7d){
            if(*(buf_ptr+1) == 0x02){
                head_ptr[i] = 0x7e;
                buf_ptr += 2;
                goto Checkout;
            }else if(*(buf_ptr+1) == 0x01){
                head_ptr[i] = 0x7d;
                buf_ptr += 2;
                goto Checkout;
            }else{
                print_zyf("body error");
                return -1;
            }
        }else if(*buf_ptr == 0x7e){
            return -1;
        }
        head_ptr[i] = *buf_ptr++;
        Checkout:
        if(i != 0){
            hex_check ^= head_ptr[i];
        }else{
            hex_check = *head_ptr;
        }
    }

    head->cmd = ntohs(head->cmd);
    head->len = ntohs(head->len);


    while(*buf_ptr != 0x7e){
        if(*buf_ptr != 0x7d){
            *body_ptr = *buf_ptr++;
        }else{
            ++buf_ptr;
            if(*(buf_ptr++) == 0x01){
                *body_ptr = 0x7d;
                
            }else {
                *body_ptr = 0x7e;
            }
        }
        hex_check ^= *body_ptr++;
    }
    *(body-1) = 0;
    if(hex_check == 0){
        return 0;
    }else{
        print_zyf("checksum error %02x", *(buf_ptr - 1));
        return -2;
    }
}


static cJSON *body_json;
static cJSON* set_json_body(char *body){
    body_json = cJSON_Parse(body);
    return body_json;
}

static cJSON* get_json_key(char *key){
    return cJSON_GetObjectItem(body_json, key);
}

static void delete_json(){
    if(body_json){
        cJSON_Delete(body_json);
    }
}

static int dis_common(char *body, int *result){
    int ansId;
    if(!set_json_body(body)){
        return -1;
    }
    ansId = GET_INT_DEFAULT(get_json_key("ansId"));
    *result = GET_INT(get_json_key("result"), 50000-1);
    delete_json();
    return ansId;
}

static int dis_ctrl(char *body, hope_ctrl_t *ctrl){
    cJSON *root = cJSON_Parse(body);
    if(!root){
        print_zyf("root json error");
        goto Exit;
    }
    cJSON *profile = cJSON_GetObjectItem(root, "profile");
    if(!profile){
        print_zyf("profile json error");
        goto Exit;
    }

    bzero(ctrl, sizeof(hope_ctrl_t));
    ctrl->status = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "status"));
    ctrl->play = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "play"));
    ctrl->ctrl = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "control"));
    ctrl->skip = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "skip"));
    ctrl->idvol = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "idvol"));
    ctrl->mute = GET_BOOL(cJSON_GetObjectItem(profile, "mute"));
    ctrl->setvol = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "setvol"));
    ctrl->source = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "source"));
    ctrl->effect = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "effect"));
    ctrl->model = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "model"));
    ctrl->locale = GET_INT_DEFAULT(cJSON_GetObjectItem(profile, "locale"));

    printf("common operation get body %s\n", body);
    printf("%d %d %d %d %d %d %d %d %d %d %d\r\n", ctrl->status, ctrl->play, ctrl->ctrl, ctrl->skip, ctrl->idvol, ctrl->mute, ctrl->setvol, ctrl->source, ctrl->effect, ctrl->model, ctrl->locale);

Exit:
    if(root)cJSON_Delete(root);

    return 0;
}

static cloud_head_t dis_head;
static char dis_body[1024];
static hope_ctrl_t ctrl;
static int dispatch_command(char *buf, int len){
    int ret = 0;
    bzero(dis_body, 1024);
    int i;
    // printf("dispatch buf: ");
    // for(i = 0; i < len; i++){
    //     printf("%d: %02x", i, buf[i]);
    // }
    // printf("\n");

    ret = dispatch_buf(len, buf, &dis_head, dis_body);
    if(ret){
        print_zyf("dispatch buf error");
        goto Exit;
    }
    // print_zyf("get body %s", dis_body);
    uint8_t cmd = dis_head.cmd & 0x0fff;
    switch(cmd){
        case C_CMD_COMMON:{
            print_zyf("common reply");
            int result;
            ret = dis_common(dis_body, &result);
            if(result != 50000){
                print_zyf("get result status %d", result);
                result -= 50000;
            }
            ret = result;
            break;
        }
        case C_CMD_HEART:{
            print_zyf("heatbeat common reply");
            break;
        }
        case C_CMD_CTRL:{
            dis_ctrl(dis_body, &ctrl);
            print_zyf("common operation");
            break;
        }
       case C_CMD_SONGLIST_NEW:{
            print_zyf("new songlist");
            break;
        }
        case C_CMD_SONGLIST_CREATE:{
            print_zyf("create songlist add song in");
            break;
        }
        case C_CMD_SONGLIST_RM:{
            print_zyf("rm songlist");
            break;
        }
        case C_CMD_SONGLIST_ADD:{
            print_zyf("add song to songlist");
            break;
        }
        case C_CMD_AREA_SCAN:{
            print_zyf("scan status of area");
            break;
        }
        case C_CMD_AREA_CTRL:{
            print_zyf("ctrl area");
            break;
        }
       case C_CMD_SONGLIST_RMSONG:{
            print_zyf("rm a song from songlist");
            break;
        }
        case C_CMD_SCENE_NEW:{
            print_zyf("create ot change scene");
            break;
        }
        case C_CMD_SCENE_RULE:{
            print_zyf("remove rule of scene");
            break;
        }
        case C_CMD_SHARE:{
            print_zyf("share dev to else");
            break;
        }
        case C_CMD_EXIT:{
            print_zyf("call to exit");
            break;
        }
        case C_CMD_COLLECT_CTRL:{
            print_zyf("collect songlist ctrl");
            break;
        }
       case C_CMD_PLAY_SONGLIST:{
            print_zyf("play songs on type");
            break;
        }
        case C_CMD_PLAY_SONGS:{
            print_zyf("play songs");
            break;
        }
        case C_CMD_ADDTO_CUR:{
            print_zyf("add songs to current songlist");
            break;
        }        
        case C_CMD_ADDTO_ELSE:{
            print_zyf("add songs to some songlist");
            break;
        }
        case C_CMD_ADDTO_NEW:{
            print_zyf("create songlist and add songs");
            break;
        }
        case C_CMD_CLEAR_SONGLIST:{
            print_zyf("cleat songlist");
            break;
        }
       case C_CMD_SCENE_CTRL:{
            print_zyf("ctrl current scene");
            break;
        }
        case C_CMD_SCENE_RM:{
            print_zyf("remove scene");
            break;
        }
        case C_CMD_SCENE_START:{
            print_zyf("start scene");
            break;
        }  
        case C_CMD_GET_STATUS:{
            print_zyf("get status of dev");
            break;
        }
        case C_CMD_SCENE_CTRLALL:{
            print_zyf("ctrl all scenes");
            break;
        }
        case C_CMD_SCENE_RMALL:{
            print_zyf("remove all scenes");
            break;
        }
        default:break;
    }
Exit:
    return ret;
}
