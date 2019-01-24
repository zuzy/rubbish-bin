#include <pthread.h>
#include <applib/msg_loop.h>
#include <case_msg.h>
#include <applib/service_api.h>
#include <applib/app_timer.h>
#include <act_debug.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>

#include "applib.h"
#include "act_debug.h"
#include "net/config.h"
#include "net/wifi_manager.h"
#ifndef off64_t
typedef unsigned long long off64_t;
#endif
#include "ghttp/ghttp.h" 
#include "cJSON.h"
#include "md5.h"
#include "common.h"
#include "http_regist.h"

#define MAX_STR_SPLIT 256
#define MAX_STR_POST_BODY 1024

static const char *ver = "1.0";
// static const char *cid = "750064224428658688";
static const char *cid = "750832243333435392";

static const char *sid = "750837261197414400";
// static const char *key = "A716A953593940D2BD78E1A02CD3C070";
static const char *key = "306BE98143724C6481E5F2BB7F6951D5";

static char post_body[MAX_STR_POST_BODY];
static const char *post_format = "ver=%s&des=%02X&cid=%s&sid=%s&key=%s&len=%s&dat=%s";
static ghttp_request *req = NULL;

static int _http_interrupt(void *param){
    return 0;
}

void post_init(char *uri){
    req = ghttp_request_new(_http_interrupt, NULL);
    ghttp_set_uri(req, uri);
    ghttp_set_type(req, ghttp_type_post);
    // ghttp_prepare(req);
}

void post_set_body(char *ver, char *cid, char *sid, char *key, int len, char *data){
    MD5_CTX md5;
    char len_str[4] = {0};
    unsigned char decrypt[16], hex_str[33], hex;
    int i;

    char *str_split = (char*)malloc(MAX_STR_SPLIT);
    bzero(str_split, MAX_STR_SPLIT);

    sprintf(len_str, "%d", len);
    sprintf(str_split, "%s%s%s%s%s", len_str, key, cid, sid, ver);
    
    MD5Init(&md5);
    MD5Update(&md5, str_split, strlen(str_split));
    MD5Final(&md5, decrypt);
    hex = decrypt[0];
    bzero(hex_str, 32);
    for(i = 0; i < 16; i++){
        sprintf(hex_str, "%s%02X", hex_str, decrypt[i]);
    }
    print_zyf("str split: %s\nmd5: %s\n", str_split, hex_str);
    hex = hex_str[0];
    for(i = 1; i < 32; i++){
        hex ^= hex_str[i];
    }
    bzero(post_body, MAX_STR_POST_BODY);
    sprintf(post_body, post_format, ver, hex, cid, sid, key, len_str, data);
    print_zyf("postbody %s", post_body);
    ghttp_set_header(req, "Content-Type", "application/x-www-form-urlencoded");
    int ret = ghttp_set_body(req, post_body, strlen(post_body));
    
    if(str_split)free(str_split);
    str_split = NULL;
    return ret;
}

int post_and_catch(char *body, int len){
    int ret = 0;
    ghttp_status status;
    ghttp_prepare(req);
    status = ghttp_connect(req, 0ll, 0ll);
    if((status != ghttp_done) && (status != ghttp_error_no_support_range)){
        print_zyf("http post fail\n");
        ret = -1;
        goto EXIT;
    }
    ret = ghttp_status_code(req);
    if(ret != 200){
        print_zyf("ghttp status error %d", ret);
        goto EXIT;
    }
    ghttp_read(req, body, len);
EXIT:
    ghttp_request_destroy(req);
    return ret;
}
union {
    int number;
    char s;
}test;

int testBig(){
    test.number = 0x01000002;
    return test.s;
}

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
    for(i = 0; i < size; i++){
        temp[i] = body[i];
        if(strstr(temp, "refrenceId\":")){
            break;
        }
    }
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
    if(temp)free(temp);
    temp = NULL;
}

int regist_device(char *auth, unsigned char *ref){
    char province[32], city[32], longitude[32], latitude[32], area[32], mac[32];
    char *body = (char*)malloc(1024);
    int ret;
    get_mac_string(mac);
    get_location(province, city, longitude, latitude, area);
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, DAT_COMNAME, R_COMPANY_NAME);
    cJSON_AddStringToObject(root, DAT_DEV_SN, R_DEVICE_SN);
    cJSON_AddStringToObject(root, DAT_DEV_NAME, R_DEVICE_NAME);
    cJSON_AddStringToObject(root, DAT_DEV_CATA, R_DEVICE_CATA);
    cJSON_AddStringToObject(root, DAT_VER_FIRM, R_FIRM_VERSION);
    cJSON_AddStringToObject(root, DAT_VER_SOFT, R_SOFT_VERSION);
    cJSON_AddStringToObject(root, DAT_MAC, mac);

    cJSON_AddStringToObject(root, DAT_PLY_TYPE, R_PLAYER_TYPE);
    cJSON_AddStringToObject(root, DAT_PLY_VER, R_PLAYER_VERSION);
    cJSON_AddStringToObject(root, DAT_BG_TYPE, R_BACKGROUND_TYPE);
    cJSON_AddStringToObject(root, DAT_BG_VER, R_BACKGROUND_VER);
    cJSON_AddStringToObject(root, DAT_UPGRADE_CH, R_UPGRADE_CHANNEL);
    cJSON_AddStringToObject(root, DAT_DEV_DRIVE, R_DEVICE_DRIVE);
    cJSON_AddStringToObject(root, DAT_OS_LANG, "zh");
    cJSON_AddStringToObject(root, DAT_LOC_LONG, longitude);
    cJSON_AddStringToObject(root, DAT_LOC_LAT, latitude);
    cJSON_AddStringToObject(root, DAT_LOC_CITY, city);
    cJSON_AddStringToObject(root, DAT_LOC_PROVINCE, province);
    cJSON_AddStringToObject(root, DAT_PARENT, R_PARENT_ID);
    cJSON_AddStringToObject(root, DAT_LOC_AREA, area);
    char *dat = cJSON_Print(root);
    post_init(REGIST_DEV_URI);
    post_set_body(ver, cid, sid, key, strlen(dat), dat);
    ret = post_and_catch(body, 1024);
    cJSON_Delete(root);
    print_zyf("regist %d, body %s", ret, body);
    root = cJSON_Parse(body);
    cJSON *code_item = cJSON_GetObjectItem(root, "code");
    if(code_item->valueint != 100000){
        print_zyf("regist failed %d", code_item->valueint);
        goto Exit;
    }
    cJSON *obj_item = cJSON_GetObjectItem(root, "object");
    cJSON *auth_item = cJSON_GetObjectItem(obj_item, "authCode");
    if(auth_item){
        print_zyf("get authcode %s", auth_item->valuestring);
        strcpy(auth, auth_item->valuestring);
    }
    get_ref_from_json(body, ref);
Exit:
    if(body)free(body);
    body = NULL;
    cJSON_Delete(root);
    return ret;
}

int regist_property(){
    int ret = -1;
    cJSON *root, *profile;
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "deviceId", "750837261197414400");
    profile = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "profile", profile);
    cJSON_AddNumberToObject(profile, "status", 1);
    cJSON_AddNumberToObject(profile, "play", 1);
    cJSON_AddNumberToObject(profile, "crrl", 1);
    cJSON_AddNumberToObject(profile, "skip", 1);
    cJSON_AddNumberToObject(profile, "idvol", 1);
    cJSON_AddBoolToObject(profile, "mute", 1);
    cJSON_AddNumberToObject(profile, "setvol", 1);
    cJSON_AddNumberToObject(profile, "source", 1);
    cJSON_AddNumberToObject(profile, "effect", 1);
    cJSON_AddNumberToObject(profile, "model", 1);
    cJSON_AddNumberToObject(profile, "locale", 1);

    char *body = (char*)malloc(1024);
    if(!body){
        goto Exit;
    }
    char *dat = cJSON_Print(root);
    if(!dat){
        goto Exit;
    }
    post_init(REGIST_PRO_URI);
    post_set_body(ver, cid, sid, key, strlen(dat), dat);
    ret = post_and_catch(body, 1024);
    cJSON_Delete(root);
    print_zyf("regist %d, body %s", ret, body);
    root = cJSON_Parse(body);
    cJSON *code_item = cJSON_GetObjectItem(root, "code");
    if(code_item->valueint != 100000){
        print_zyf("regist failed %d", code_item->valueint);
        goto Exit;
    }
Exit:
    if(body)free(body);
    body = NULL;
    cJSON_Delete(root);
    return ret;
}


