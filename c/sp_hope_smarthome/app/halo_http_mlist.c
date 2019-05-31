#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <apconfig.h>
#include <syslog.h>
#include <pthread.h>
#include <stdbool.h>
#include "applib/app_config.h"
#include "net/wifi_manager.h"
#include "md5.h"
#include "list.h"
#include "cJSON.h"
#include "common.h"
#include "main_loop.h"
#include "halo_http.h"
#include "halo_product.h"
#include "halo_http_regist.h"
#include "tcp_cli_dispatch.h"

#include "media_navi_api.h"
#include "smart_player_api.h"
#include "smart_player_hal.h"
#include "music_player_api.h"


struct _st_mlist_post
{
    struct list_head *posts;
    struct list_head *bodies;
    cJSON *root;
    char *out;
    int busy;
} mpost_handle = {
    NULL,
    NULL,
    NULL,
    0,
};

uint64_t _now = 0;

int get_items()
{
	if(get_now_us() - _now < 500000)
    {
		return -1;
	}
	_now = get_now_us();
    char filter[100];
    char dir_filter[50];    
    char play_path[50] = {0};
    char dir_tmp[50];
    int ret;
    // get_config(CFG_APP_SUPPORT_EXT_MUSIC, filter, CFG_MAX_BUFF_LENGTH);
    if(halo_handle->main_disk < 0) {
        get_config(CFG_MAIN_DISK, &halo_handle->main_disk, sizeof(int));
    }
    switch(halo_handle->main_disk) {
        case HOTPLUGIN_DEV_TYPE_LOCAL :{
            strcpy(play_path, "/mnt/udisk");
            break;
        }
        case HOTPLUGIN_DEV_TYPE_CARD: {
            strcpy(play_path, "/mnt/sdcard");
            break;
        }
        case HOTPLUGIN_DEV_TYPE_UHOST: {
            strcpy(play_path, "/mnt/external");
            break;
        }
        default: break;
    }



    get_config(CFG_APP_SUPPORT_EXT_MUSIC, filter, CFG_MAX_BUFF_LENGTH);   
	if(halo_handle->mlist != NULL) {
		media_dir_list_close(halo_handle->mlist);
		halo_handle->mlist = NULL;
	}
	halo_handle->mlist = media_dir_open(play_path, filter, 1, 0);
	if(halo_handle->mlist == NULL) {
		print_http("media open error!!");
		return -1;
	}

	get_config(CFG_SIFANGYUEKU_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);
	
	get_config(CFG_LANMANZHIYU_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);
	
	get_config(CFG_WENXINSHIKE_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);
	
	get_config(CFG_JIZHIHAIFAN_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);
	
	get_config(CFG_JINGMISHIGUANG_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);
	
	get_config(CFG_HUIYIJINGDIAN_PATH, dir_filter, CFG_MAX_BUFF_LENGTH);
	sprintf(dir_tmp, "%s/%s", play_path, dir_filter);
	ret = media_dir_filter_dir(halo_handle->mlist, dir_tmp);

    return 0;
}


static void _init_callback(cJSON *root)
{
    if(strlen(halo_handle->auth) == 0 || strlen(halo_handle->ref_ori) == 0) {
        get_config(CFG_HREG_AUTH, halo_handle->auth, 33);
        get_config(CFG_HREG_REF_ORI, halo_handle->ref_ori, 21);
    }
    cJSON_AddStringToObject(root, "authCode", halo_handle->auth);
    cJSON_AddStringToObject(root, "deviceId", halo_handle->ref_ori);
    print_regist("%s: %s", halo_handle->auth, halo_handle->ref_ori);
}

static void _add_callback(cJSON *list, int index, media_item_t item)
{
    mp_id3_t id3_info;
    int msec = 0;
    memset(&id3_info, 0, sizeof(id3_info));
    // mp_get_info_init();
    mp_get_id3_from_uri(&id3_info, &msec, item.url);
    // mp_get_info_end();

	char title[256] = "";
    
    get_music_name(item.url, title, sizeof(title)-1);

    cJSON *song = cJSON_CreateObject();

    if(strlen(id3_info.album)){
        utf8_cut_string(id3_info.album, 63);
        cJSON_AddStringToObject(song, "albumName", id3_info.album);
    }else{
        cJSON_AddStringToObject(song, "albumName", "");
    }
    if(strlen(id3_info.author)){
        utf8_cut_string(id3_info.author, 63);
        cJSON_AddStringToObject(song, "authorName", id3_info.author);
    }else{
        cJSON_AddStringToObject(song, "authorName", "");
    }
    if(strlen(id3_info.title)){
        utf8_cut_string(id3_info.title, 127);
        cJSON_AddStringToObject(song, "displayName", id3_info.title);
        utf8_cut_string(id3_info.title, 63);
        cJSON_AddStringToObject(song, "musicName", id3_info.title);
    }else{
        utf8_cut_string(title, 127);
        cJSON_AddStringToObject(song, "displayName", title);
        utf8_cut_string(title, 63);
        cJSON_AddStringToObject(song, "musicName", title);
    }
    cJSON_AddNumberToObject(song, "musicId", index);
    cJSON_AddNumberToObject(song, "musicTime", msec);
    cJSON_AddItemToArray(list, song);

}

static cJSON *_muisclist_build(void *fd, int start, int times, void *initcallback(cJSON *), void *addcallback(cJSON *, int, media_item_t)){
    cJSON *root = cJSON_CreateObject();
    initcallback(root);
    int i = 0;
    media_item_t item;
    char *out = NULL;
    cJSON *list = cJSON_CreateArray();
    cJSON *u_root = cJSON_CreateObject();
    cJSON *u_list = cJSON_CreateArray();
    print_http("start:%d times:%d", start, times);
    for(i; i < times; i++){
        if(media_get_items(fd, start+i, 1, &item) > 0){
            addcallback(list, start+i, item);
            cJSON_AddItemToArray(u_list, cJSON_CreateString(item.url));
        }else{
            goto Error_build;     
        }
        usleep(10000);
    }
    cJSON_AddItemToObject(u_root, "array", u_list);
    out = cJSON_PrintUnformatted(u_root);
    print_http("uri list %s", out);
    lua_call_fun("/tmp/lua/mlist.lua", "save_uri_list", "s", out);
    cJSON_Delete(u_root);
    free(out);
    u_list = out = NULL;
    cJSON_AddItemToObject(root, "list", list);
    return root;
Error_build:
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    if(u_list != NULL) {
        cJSON_Delete(u_list);
        u_list = NULL;
    }
    if(out != NULL) {
        free(out);
        out = NULL;
    }
    return NULL;
}

static int resultcb(void *arg, void *arg_post)
{
    int ret = 0;
    st_post *post = (st_post *)arg_post;
    st_result *result = (st_result *)arg;
    print_regist("get result %d: %s", result->code, result->body);
    cJSON *root = cJSON_Parse(result->body);

    cJSON *code_item = cJSON_GetObjectItem(root, "code");
    if(code_item->valueint != 100000){
        print_regist("regist failed %d", code_item->valueint);
        ret = -1;
    }
    cJSON_Delete(root);
    return ret;
}

static void *mlist_update_proc(void *arg)
{
    uint64_t t = get_now_us();
    
    void *mlist = arg;
    print_http("mlist %d", mlist); 
    struct list_head bodies = LIST_HEAD_INIT(bodies);
    struct list_head posts = LIST_HEAD_INIT(posts);
    int wait = 10;
    char *out = NULL;
    cJSON *root = NULL;
    int ret = 0;
    mpost_handle.busy = 1;
    print_http();
    int total, dirnum, finish;

    while(is_http_busy()) {
        print_regist("wait for http finish");
        sleep(1);
    }
    print_http();
    while(wait-- && (media_get_item_num(mlist, &total, &dirnum, &finish) == 0)) {
        if(finish) {
            mp_get_info_init();
            root = _muisclist_build(mlist, 0, total, _init_callback, _add_callback);
            mp_get_info_end();
            break;
        }
        sleep(1);
    }
    out = cJSON_PrintUnformatted(root);

    lua_call_fun("/tmp/lua/mlist.lua", "savelist", "s", out);

    print_regist("out is %d %s", utf8_strlen(out), out);
    init_bodies(&bodies, out, utf8_strlen(out));
    add_post_hash_bodies(&posts, "http://open.nbhope.cn/hopeApi/music/initial", NULL, &bodies, resultcb);
    
    free_hash(&bodies);
    cJSON_Delete(root);
    free(out);
    init_http(&posts, 3, 0, NULL, NULL);
    halo_http_start();
    while(1) {
        ret = halo_http_result();
        if(ret != 1) {
            break;
        }
        print_regist("wait for regist done");
        sleep(1);
    }
    deinit_http();

    mpost_handle.busy = 0;

    print_http("mlist regist %d us", get_now_us() - t);

    pthread_exit((void *) 0);
}

int mlist_update(void *mlist)
{
    pthread_t tid;
    wifi_status_e status;
	hotplug_get_wifi_status(&status);
    if (status != WIFI_STATE_CONNECTED) {
        return -1;
    }
    if(mpost_handle.busy == 1) {
        print_regist("busy");
        return -1;
    }
    print_http("create thread! %d", mlist);
    pthread_create(&tid, NULL, mlist_update_proc, mlist);
    pthread_detach(tid);
    return 0;
}