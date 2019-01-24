#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <apconfig.h>
#include <syslog.h>
#include "applib/app_config.h"
#include "md5.h"
#include "list.h"
#include "cJSON.h"
#include "common.h"
#include "halo_http.h"
#include "halo_product.h"
#include "halo_http_regist.h"

static int resultcb(void *arg, void *arg_post)
{
    st_post *post = (st_post *)arg_post;
    st_result *result = (st_result *)arg;
    print_regist("get result %d: %s", result->code, result->body);
    cJSON *root = cJSON_Parse(result->body);

    cJSON *code_item = cJSON_GetObjectItem(root, "code");
    if(code_item->valueint != 100000){
        print_regist("regist failed %d", code_item->valueint);
        cJSON_Delete(root);
        return -1;
    }
    cJSON_Delete(root);
    return 0;
}

static int __init_bodies(struct list_head *bodies, char *data, int len)
{
    MD5_CTX md5;
    unsigned char decrypt[16] = {0};
    char hex_str[33] = "";
    unsigned char hex = 0;
    char len_str[5] = "";
    char des_str[5] = "";
    int i;

    sprintf(len_str, "%d", len);
    size_t l = strlen(len_str) + strlen(HALO_KEY) + strlen(HALO_CID) + strlen(HALO_SID) + strlen(HALO_VER) + 1;
    char *str_split = (char*)malloc(l);
    if(str_split == NULL){
        print_regist("malloc failed");
        return -1;
    }
    bzero(str_split, l);

    sprintf(str_split, "%s%s%s%s%s", len_str, HALO_KEY, HALO_CID, HALO_SID, HALO_VER);
    MD5Init(&md5);
    MD5Update(&md5, (unsigned char*)str_split, strlen(str_split));
    MD5Final(&md5, decrypt);
    hex = decrypt[0];
    bzero(hex_str, sizeof(hex_str));
    for(i = 0; i < 16; i++){
        sprintf(hex_str, "%s%02X", hex_str, decrypt[i]);
    }
    hex = hex_str[0];
    for(i = 1; i < 32; i++){
        hex ^= hex_str[i];
    }
    sprintf(des_str, "%.02X", hex);
    add_hash(bodies, "ver", HALO_VER);
    add_hash(bodies, "des", des_str);
    add_hash(bodies, "cid", HALO_CID);
    add_hash(bodies, "sid", HALO_SID);
    add_hash(bodies, "key", HALO_KEY);
    add_hash(bodies, "len", len_str);
    add_hash(bodies, "dat", data);
    free(str_split);
    return 0;
}

int regist_mac(struct list_head *posts)
{
    char mac[20] = "";
    struct list_head bodies = LIST_HEAD_INIT(bodies);
    
    get_mac_string(mac);
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "deviceGuid", mac);
    cJSON_AddStringToObject(root, "deviceSN", HALO_SN);
    char *out = cJSON_PrintUnformatted(root);

    __init_bodies(&bodies, out, strlen(out));
    add_post_hash_bodies(posts, "http://192.168.2.9:8080/hopeApi/upgrade/mac", NULL, &bodies, resultcb);
    free_hash(&bodies);
    free(out);
    cJSON_Delete(root);
    return 0;
}

static unsigned char __my_htoi(char *input){
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

int get_ref_from_json(char *body){

    int i, size;
    char *ptr = NULL;
    ptr = strstr(body, "refrenceId\":");
    if(ptr == NULL){
        print_regist("null!!");
        return -1;
    }
    while(*ptr != '-' && (*ptr > '9' || *ptr < '0')){
        ++ptr;
    }
    char ref[100] = "";
    char *p_ref = ref;
    while((*ptr <= '9' && *ptr >= '0') || *ptr == '-'){
        *p_ref++ = *ptr++;
    }
    // set config ref
    set_config(CFG_HALO_REF_ORI, ref, MAX_ID_LEN);


    p_ref = ref;
    char ref_str[100] = "";
    char *p_ref_str = ref_str;
    int len = strlen(p_ref);
    if(p_ref[0] != '-'){
        if(len > 20){

        }else {
            if(len < 20){
                int i , size;
                size = 20 - len;
                for(i = 0; i < size; i++){
                    *p_ref_str++ = 'A';
                }
            }
            strcpy(p_ref_str, p_ref);
        }
    }else{
        if(len > 20){

        }else{
            if(len < 20){
                int size = 21 - len; // 20 - len + 1 the '-'
                int i;
                for(i = 0; i < size; i++){
                    *p_ref_str++ = 'F';
                }
                strcpy(p_ref_str, p_ref+1);
            }else{
                strcpy(p_ref_str, p_ref);
                p_ref_str[0] = 'F';
            }
        }
    }
    set_config(CFG_HALO_REF_STR, ref_str, MAX_ID_LEN);

    unsigned char ref_hex[100] = {0};
    char hex_num[3] = {0};
    for(i = 0; i < 10; i++){
        hex_num[0] = ref_str[(i<<1)];
        hex_num[1] = ref_str[(i<<1)+1];
        ref_hex[i] = __my_htoi(hex_num);
    }
    set_config(CFG_HALO_REF_HEX, ref_hex, MAX_ID_LEN);
    sync_config();
    return 0;
}


static int __regist_dev_cb(void *arg, void *arg_post)
{
    st_post *post = (st_post *)arg_post;
    st_result *result = (st_result *)arg;
    print_regist("get result %d: %s", result->code, result->body);
    cJSON *root = cJSON_Parse(result->body);

    cJSON *code_item = cJSON_GetObjectItem(root, "code");
    if(code_item->valueint != 100000){
        print_regist("regist failed %d", code_item->valueint);
        goto Error;
    }
    cJSON *obj_item = cJSON_GetObjectItem(root, "object");
    cJSON *auth_item = cJSON_GetObjectItem(obj_item, "authCode");
    char auth[100];
    if(auth_item){
        print_regist("get authcode %s", auth_item->valuestring);
        strncpy(auth, auth_item->valuestring, 100);
        set_config(CFG_HALO_AUTH, auth, MAX_ID_LEN);
    }
    get_ref_from_json(result->body);
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    // add new regist to the post list dynamicly
    regist_join_dy(post);
    // the last addition is the nearest node to post
    regist_property_dy(post);
    return 0;
Error:
    if(root != NULL) {
        cJSON_Delete(root);
    }
    return -1;
}


int regist_device(struct list_head *posts)
{
    char mac[20] = "";
    struct list_head bodies = LIST_HEAD_INIT(bodies);
    get_mac_string(mac);
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, DAT_COMNAME, R_COMPANY_NAME);
    cJSON_AddStringToObject(root, DAT_DEV_SN, HALO_SN);
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
    cJSON_AddStringToObject(root, DAT_PARENT, R_PARENT_ID);
    char *out = cJSON_PrintUnformatted(root);
    __init_bodies(&bodies, out, strlen(out));
    add_post_hash_bodies(posts, "http://192.168.2.9:8080/hopeApi/device/register", NULL, &bodies, __regist_dev_cb);
    free_hash(&bodies);
    cJSON_Delete(root);
    free(out);
    return 0;
}

int regist_property_dy(st_post *post)
{
    char mac[20] = "";
    struct list_head bodies = LIST_HEAD_INIT(bodies);
    get_mac_string(mac);
    cJSON *root, *profile;
    root = cJSON_CreateObject();
    char refid[MAX_ID_LEN] = "";
    get_config(CFG_HALO_REF_ORI, refid, MAX_ID_LEN);
    cJSON_AddStringToObject(root, "deviceId", refid);
    profile = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "profile", profile);
    cJSON_AddNumberToObject(profile, "status", 1);
    cJSON_AddNumberToObject(profile, "play", 1);
    cJSON_AddNumberToObject(profile, "control", 1);
    cJSON_AddNumberToObject(profile, "skip", 1);
    cJSON_AddNumberToObject(profile, "idvol", 1);
    cJSON_AddBoolToObject(profile, "mute", 1);
    cJSON_AddNumberToObject(profile, "setvol", 1);
    cJSON_AddNumberToObject(profile, "source", 1);
    cJSON_AddNumberToObject(profile, "effect", 1);
    cJSON_AddNumberToObject(profile, "model", 1);
    cJSON_AddNumberToObject(profile, "locale", 1);
    char *out = cJSON_PrintUnformatted(root);
    // print_regist("out is %s", out);
    __init_bodies(&bodies, out, strlen(out));
    add_post_hash_bodies_dy(post, "http://192.168.2.9:8080/hopeApi/device/status", NULL, &bodies, resultcb);
    free_hash(&bodies);
    cJSON_Delete(root);
    free(out);
    return 0;
}

int regist_join_dy(st_post *post)
{
    struct list_head bodies = LIST_HEAD_INIT(bodies);
    char *out = NULL;
    cJSON *root = cJSON_CreateObject();
    if(root == NULL) {
        goto Error;
    }
    char refid[MAX_ID_LEN] = "";
    get_config(CFG_HALO_REF_ORI, refid, MAX_ID_LEN);
    if(refid[0] == '\0') {
        print_regist("get refid failed");
        goto Error;
    }
    char auth[MAX_ID_LEN] = "";
    get_config(CFG_HALO_AUTH, auth, MAX_ID_LEN);
    if(auth[0] == '\0') {
        print_regist("get auth failed");
        goto Error;
    }
    char uuid[64] = "";
    get_config("ALI_AI_UUID", uuid, sizeof(uuid));
    if(uuid[0] = '\0') {
        print_regist("get uuid failed");
        goto Error;
    }
    cJSON_AddStringToObject(root, "deviceId", refid);
    cJSON_AddStringToObject(root, "authCode", auth);
    cJSON_AddStringToObject(root, "platformId", "774702725827940352");
    cJSON_AddStringToObject(root, "uuid", uuid);
    out = cJSON_PrintUnformatted(root);
    if(out == NULL) {
        goto Error;
    }
    __init_bodies(&bodies, out, strlen(out));
    add_post_hash_bodies_dy(post, "http://192.168.2.9:8080/hopeApi/device/editJoin", NULL, &bodies, resultcb);
    free_hash(&bodies);
    cJSON_Delete(root);
    free(out);
    return 0;
Error:
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    if(out != NULL) {
        free(out);
        out = NULL;
    }
    return -1;
}

int regist_first()
{
    print_regist("init2 !!!!!!");
    struct list_head posts = LIST_HEAD_INIT(posts);
    regist_mac(&posts);
    regist_device(&posts);

    init_http(&posts, 3, 0, NULL, NULL);
    halo_http_start();
    // sleep(5);
    int re = halo_http_result();
    print_regist("regist result is %d", re);
    deinit_http();
    if(re != 0) {
        return ERR_FORCE_INIT;
    }
    return ERR_IGN;
}

