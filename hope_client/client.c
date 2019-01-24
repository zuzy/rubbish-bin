#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> // needed for hostent; gethostbyname

#include "cloud_client.h"
#include "cJSON.h"

#define GET_INT(json,a) ((json)?((json)->valueint):(a))
#define GET_INT_DEFAULT(json)   GET_INT(json,-1)
#define GET_BOOL(json) ((json)?((json)->type):-1)

static int sockfd = -1;
static char *buf[2024];
static int num;
static volatile int recv_status;
static cloud_head_t local_head;
static pthread_t client_tid;
pthread_mutex_t client_lock;
static char auth_code[33];
static unsigned char ref_id[11];
void *send_thread(void *arg);
/*
[***zyf***]:http_regist.c,241--- regist 200, body {"code":100000,"message":"成功！","object":{"authCode":"0CAFE38ADB1E43C3BB9AC5A701E70C5C","refrenceId":754121335794077696}}
[***zyf***]:http_regist.c,250--- get authcode 0CAFE38ADB1E43C3BB9AC5A701E70C5C
[***zyf***]:http_regist.c,170--- ref id is 754121335794077696
[***zyf***]:http_regist.c,197--- temp changed is AA754121335794077696
*/
static int identify();
static int dispatch_command(char *buf, int len);


#define print_zyf(format, arg...)   do { printf("[***zyf***]:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                    while (0)

unsigned char my_htoi(char *input){
    unsigned char ret = 0;
    char *str = input;
    while(*str){
        ret <<= 4;
        if(*str <= '9' && *str >= '0'){
            ret += *str-'0';
        }else if(*str <= 'f' && *str >= 'a'){
            ret += 10 + *str - 'a';
        }else if(*str <= 'F' && *str >= 'A'){
            ret += 10 + *str - 'A';
        }else{
            break;
        }
        ++str;
    }
    return ret;
}

int get_ref_from_json(char *body, unsigned char *ref){
    int i, size;
    unsigned char *temp = (unsigned char *)malloc(512);
    size = strlen(body);
    i = 0;
    bzero(temp, 512);
    int j;
    for(; i < size; i++){
        if(body[i] == '-' || (body[i] <= '9' && body[i] >= '0')){
            break;
        }
    }
    if(body[i] == '-'){
        j = 1;
        temp[0] = '-';
    }else{
        j = 0;
    }
    for(j; i < size; i++, j++){
        if(body[i] <= '9' && body[i] >= '0'){
            temp[j] = body[i];
        }else{
            break;
        }
    }
    print_zyf("ref id is %s", temp);

    if(temp[0] != '-'){
        if(strlen(temp) < 20){
            size = strlen(temp);
            i = 20 - size;
            for(j = 19; j >= i; j--){
                temp[j] = temp[j-i];
            }
            for(j = 0; j < i; j++){
                temp[j] = 'A';
            }
        }
    }else{
        if(strlen(temp) < 20){
            size = strlen(temp);
            i = 20 - size;
            for(j = 19; j > i; j--){
                temp[j] = temp[j-i];
            }
            for(j = 0; j <= i; j++){
                temp[j] = 'F';
            }
        }else{
            temp[0] = 'F';
        }
    }
    print_zyf("temp changed is %s", temp);
    char hex_num[3] = {0};
    for(i = 0; i < 10; i++){
        hex_num[0] = temp[(i<<1)];
        hex_num[1] = temp[(i<<1)+1];
        ref[i] = my_htoi(hex_num);
    }
    free(temp);
}

static void *client_proc(void *arg){
    arg = NULL;
    struct sockaddr_in server;
    pthread_t tid;
    pthread_mutex_init(&client_lock, NULL);

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
    print_zyf("connect ok");
    pthread_create(&tid, NULL, send_thread, NULL);
    identify();
    while(1){
        num = recv(sockfd, buf, 2024, 0);
        print_zyf("recv %d", num);
        if(num <= 0){
            print_zyf("recv error");
            break;
        }else{
            dispatch_command(buf, num);
        }
    }
Exit:
    pthread_mutex_destroy(&client_lock);
    close(sockfd);
    sockfd = -1;
}

int cloud_client_start(){
    int ret = -1;
    bzero(auth_code, 33);
    bzero(ref_id, 11);
#if 0
    if(regist_device(auth_code, ref_id) == 200){
        print_zyf("regist ok");
        ret = pthread_create(&client_tid, NULL, client_proc, NULL);
    }else{
        print_zyf("regist error");
    }
#else
    strcpy(auth_code, "0CAFE38ADB1E43C3BB9AC5A701E70C5C");
    get_ref_from_json("754121335794077696", local_head.snd_id);
    client_proc(NULL);
#endif
    return ret;
}

int cloud_client_close(){
    if(client_tid >= 0)
        return pthread_join(client_tid, NULL);
    else
        return -1;
}

// body is json no need to 7e 7d
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

static int cloud_send(uint16_t cmd, char *input){
    local_head.cmd = htons(cmd);
    local_head.type = 1;
    local_head.len = htons(strlen(input)<<1);

    return _send(local_head, input);
}

static int identify(){
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "code", auth_code);
    return cloud_send(C_DEV_IDENTIFY, cJSON_Print(root));
    cJSON_Delete(root);
}

static int heart_beat(){
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
    printf("get head:\r\n");
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
        printf(" %02x", *buf_ptr);
        head_ptr[i] = *buf_ptr++;
    Checkout:
        if(i != 0){
            hex_check ^= head_ptr[i];
        }else{
            hex_check = *head_ptr;
        }
    }
    printf("\r\n");

    head->cmd = ntohs(head->cmd);
    head->len = ntohs(head->len) >> 1;
    printf("head cmd:%04x len:%d\r\n", head->cmd, head->len);

    // size = head->len;
    // memcpy(body, buf_ptr, size);
    // printf("body: %s\n", body);
    // for(i = 0; i < size; i++){
    //     hex_check ^= body[i];
    //     printf("%02x ", body[i]);
    // }
    // printf("\n");
    //
    // buf_ptr += size;
    // printf("hex check %02x %02x %02x %02x\r\n", hex_check, *buf_ptr, *(buf_ptr+1), *(buf_ptr+2));
    // if(*buf_ptr == 0x7d){
    //     buf_ptr++;
    //     if(*buf_ptr == 1){
    //         hex_check ^= 0x7d;
    //     }else{
    //         hex_check ^= 0x7e;
    //     }
    // }else{
    //     if(*buf_ptr != 0x7e){
    //         hex_check ^= *buf_ptr;
    //     }
    // }

    printf("get body: \n");
    i = 0;
    for(i = 0; i < size; i++){
        if(*buf_ptr != 0x7d){
            body[i] = *buf_ptr++;
        }else{
            ++buf_ptr;
            if(*(buf_ptr++) == 0x01){
                body[i] = 0x7d;
            }else {
                body[i] = 0x7e;
            }
        }
        printf("(%d %x)", i, body[i]);
        hex_check ^= body[i];
    }
    // while(*buf_ptr != 0x7e){
    //     if(*buf_ptr != 0x7d){
    //         *body_ptr = *buf_ptr++;
    //     }else{
    //         ++buf_ptr;
    //         if(*(buf_ptr++) == 0x01){
    //             *body_ptr = 0x7d;
    //
    //         }else {
    //             *body_ptr = 0x7e;
    //         }
    //     }
    //     printf("(%d %x)", i++, *body_ptr);
    //     hex_check ^= *body_ptr++;
    // }
    printf("\n\n");
    *(body-2) = 0;
    print_zyf("body is %s", body);
    if(hex_check == 0){
        print_zyf("checksum ok");
        return 0;
    }else{
        print_zyf("checksum error %02x", *(body_ptr - 1));
        return -2;
    }
}

static int dis_common(char *body, int *result){
    int ansId = -1;
    cJSON *root = cJSON_Parse(body);
    if(!root){
        return -1;
    }
    // cJSON *ans_item = cJSON_GetObjectItem(root, "ansId");
    // if(!ans_item){
    //     goto Exit;
    // }
    // cJSON *result_item = cJSON_GetObjectItem(root, "result");
    *result = GET_INT_DEFAULT(cJSON_GetObjectItem(root, "result"));
    ansId = GET_BOOL(cJSON_GetObjectItem(root, "ansId"));
    // if(!result_item){
    //     goto Exit;
    // }
    // *result = result_item->valueint;
    // ansId = ans_item->valueint;
Exit:
    cJSON_Delete(root);
    return ansId;
}

static int dispatch_command(char *buf, int len){
    int ret = 0;
    cloud_head_t *head = (cloud_head_t*)malloc(sizeof(cloud_head_t));
    char *body = (char*)malloc(1024);

    ret = dispatch_buf(len, buf, head, body);
    if(ret){
        print_zyf("dispatch buf error %d", ret);
        goto Exit;
    }
    uint8_t cmd = head->cmd & 0x0fff;
    switch(cmd){
        case C_CMD_COMMON:{
            print_zyf("common reply");
            int result;
            ret = dis_common(body, &result);
            print_zyf("ret %d; res %d", ret, result);
            if(result != 50000){
                print_zyf("get result status %d", result);
                result -= 50000;
            }
            ret += result;
            break;
        }
        case C_CMD_HEART:{
            print_zyf("heatbeat common reply");
            break;
        }
        case C_CMD_CTRL:{
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
    }
Exit:
    free(body);
    return ret;
}

void *send_thread(void *arg){
    arg = NULL;
    int n;
    while(scanf("%d",&n)!=EOF){
        switch(n){
            case 0:{
                identify();
                break;
            }
            case 1:{
                heart_beat();
                break;
            }
            case 2:{
                break;
            }
            case 3:{
                break;
            }
        }
    }
}

int main(int argc, char *argv[]){
    cloud_client_start();
}
