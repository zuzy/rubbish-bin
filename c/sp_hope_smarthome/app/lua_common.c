#include "apconfig.h"
#include "music_player_api.h"
#include "systime_api.h"
#include "lua_common.h"
#include "halo_hal.h"
#include "main_loop.h"
#include "halo_utf8.h"

static lua_State *_L = NULL;

static char *_get_member(char *obj)
{
    char *out = NULL;
    out = strrchr(obj, '.');
    return (out == NULL)? obj : ++out;
}

#define GET_MEMBER(arg) _get_member(#arg)
#define PUSH_MEMBER_TAB(L, arg)     do{ \
                        lua_pushstring(L, GET_MEMBER(arg)); \
                        lua_pushinteger(L, arg); \
                        lua_settable(L, -3); \
                        }while(0)
#define PUSH_MEMBER_STR_TAB(L, arg)     do{ \
                        lua_pushstring(L, GET_MEMBER(arg)); \
                        lua_pushstring(L, arg); \
                        lua_settable(L, -3); \
                        }while(0)
#define INIT_LUA_CBS(cb)     {#cb, cb}

#define LUA_CBS_BUILD(fun, cb, type, type_l)   \
static int fun(lua_State *L) { \
    type arg = luaL_check##type_l(L, 1); \
    lua_pushinteger(L, cb(arg)); \
    return 1;\
}


/*
    lua_init_msg_api(L);
    lua_init_player_api(L);
    lua_init_wifi_api(L);
    lua_init_config_api(L);
    lua_init_mlist_api(L);
*/

/** 
 * @brief  message api 
 * @note   applib msg 
 */

static int lua_broadcast_msg(lua_State *L)
{
    msg_apps_t msg;
    msg.type = luaL_checkint(L, 1);
    strncpy(msg.content, luaL_checkstring(L, 2), sizeof(msg.content));
    lua_pushinteger(L, broadcast_msg(&msg));
    return 1;
}

static int lua_send_async_msg(lua_State *L)
{
    msg_apps_t msg;
    char *target = luaL_checkstring(L, 1);
    msg.type = luaL_checkint(L, 2);
    strncpy(msg.content, luaL_checkstring(L, 3), sizeof(msg.content));
    lua_pushinteger(L, send_async_msg(target, &msg));
    return 1;
}

static int lua_send_sync_msg(lua_State *L)
{
    msg_apps_t msg;
    msg_reply_t reply;
    struct timespec timeout;
    char *target = luaL_checkstring(L, 1);
    msg.type = luaL_checkint(L, 2);
    strncpy(msg.content, luaL_checkstring(L, 3), sizeof(msg.content));
    int to = luaL_checkint(L, 4);
    timeout.tv_sec = to >> 10;
    timeout.tv_nsec = (to & 0x3ff) << 10;
    int ret = send_sync_msg(target, &msg, &reply, &timeout);
    if(ret) {
        lua_pushstring(L, reply.content);
        lua_pushinteger(L, reply.type);
    }
    return 2;
}

static const luaL_reg _msg_M[] =  {
    {"broadcast",       lua_broadcast_msg},
    {"send_async",      lua_send_async_msg},
    {"send_sync",       lua_send_sync_msg},
    {NULL,              NULL},
};

static void hpio_open_msg(lua_State *L)
{
    luaL_newmetatable(L, HPIO_MSG);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _msg_M);
    lua_setfield(L, -2, "msg");
}

/** 
 * @brief  player api
 * @note   smart player hal
*/

#if 0
LUA_CBS_BUILD(lua_player_loop_mode, halo_loop_set, int, int)
#else
LUA_CBS_BUILD(lua_player_loop_mode, splayer_set_loop_mode, int, int)
#endif
LUA_CBS_BUILD(lua_player_set_file, splayer_set_file, char*, string)
LUA_CBS_BUILD(lua_player_set_index, hope_music_play_index, int, int)
LUA_CBS_BUILD(lua_player_set_local_index, halo_local_index, int, int)
LUA_CBS_BUILD(lua_player_seek, halo_seek, int, int)
LUA_CBS_BUILD(lua_player_mute, halo_mute, int, int)
LUA_CBS_BUILD(lua_player_vol_ctrl, halo_vol_ctrl, int, int)
LUA_CBS_BUILD(lua_player_cut, halo_cut, int, int)
LUA_CBS_BUILD(lua_player_play, halo_play, int, int)

int lua_player_get_status(lua_State *L)
{
    splayer_status_t status;
    splayer_info_t info;
    splayer_get_status(&status);
    splayer_get_info(&info);
    lua_newtable(L);
    PUSH_MEMBER_TAB(L, status.mode);
    PUSH_MEMBER_TAB(L, status.status);
    PUSH_MEMBER_TAB(L, status.current_time);
    PUSH_MEMBER_TAB(L, status.volume);
    PUSH_MEMBER_TAB(L, status.playlist_index);
    PUSH_MEMBER_TAB(L, info.total_time);
    PUSH_MEMBER_STR_TAB(L, info.title);
    PUSH_MEMBER_STR_TAB(L, info.album);
    PUSH_MEMBER_STR_TAB(L, info.uri);
    PUSH_MEMBER_STR_TAB(L, info.author);
    return 1;    
}

int lua_player_vol(lua_State *L)
{
    int vol = luaL_checkint(L, 1);
    int persent = luaL_checkint(L, 2);
    lua_pushinteger(L, halo_vol(vol, persent));
    return 1;
}

int lua_check_alarm(lua_State *L)
{
    lua_pushboolean(L, check_is_alarm());
    return 1;
}

int lua_player_background(lua_State *L)
{
    play_list_msg();
    return 0;
}

int lua_player_play_list(lua_State *L)
{
    // char *arr_str = lua_pushstring(L, 1);
    // if(halo_handle ->local_list) 
    return 0;
}

static const luaL_reg _player_M[] =  {
    {"loop",            lua_player_loop_mode},
    {"play_file",       lua_player_set_file},
    {"__mul",           lua_player_set_index},
    {"index",           lua_player_set_index},
    {"local_index",     lua_player_set_local_index},
    {"__add",           lua_player_seek},
    {"seek",            lua_player_seek},
    {"mute",            lua_player_mute},
    {"vol_ctrl",        lua_player_vol_ctrl},
    {"cut",             lua_player_cut},
    {"play",            lua_player_play},
    {"status",          lua_player_get_status},
    {"vol",             lua_player_vol},
    {"check_alarm",     lua_check_alarm},
    {"background",      lua_player_background},
    {NULL,              NULL},
};

static void hpio_open_player(lua_State *L)
{
    luaL_newmetatable(L, HPIO_PLAYER);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _player_M);
    lua_setfield(L, -2, "player");
}

/** 
 * @brief  wifi api
 * @note   about wifi
 */

static int _wifi_info(lua_State *L)
{
    wifi_info_t info;
    wifi_get_wifi_info(&info);
    lua_newtable(L);
    PUSH_MEMBER_TAB(L, info.status);
    PUSH_MEMBER_TAB(L, info.rssi);
    PUSH_MEMBER_STR_TAB(L, info.bssid);
    PUSH_MEMBER_STR_TAB(L, info.ssid);
    PUSH_MEMBER_STR_TAB(L, info.mac_address);
    PUSH_MEMBER_STR_TAB(L, info.ip_address);
    return 1;    
}

static const luaL_reg _wifi_M[] =  {
    {"info",    _wifi_info},
    {NULL,      NULL},
};

static void hpio_open_wifi(lua_State *L)
{
    luaL_newmetatable(L, HPIO_WIFI);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _wifi_M);
    lua_setfield(L, -2, "wifi");
}

/** 
 * @brief  config api
 * @note   applib config
 */
static int lua_get_config(lua_State *L)
{
    char *key = luaL_checkstring(L, 1);
    int len = luaL_checkint(L, 2);
    char *buff = malloc(len);
    get_config(key, buff, len);
    lua_pushlstring(L, buff, len);
    free(buff);
    return 1;
}

static int lua_get_config_int(lua_State *L)
{
    char *key = luaL_checkstring(L, 1);
    int n = 0;
    int ret = get_config(key, &n, sizeof(n));
    if(ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, n);
    }
    return 1;
}

static int lua_set_config(lua_State *L)
{
    char *key = luaL_checkstring(L, 1);
    char *body = luaL_checkstring(L, 2);
    int len = luaL_checkint(L, 3);
    len = len > 0 ? len : strlen(body);
    lua_pushinteger(L, set_config(key, body, len));
}

static int lua_set_config_int(lua_State *L)
{
    char *key = luaL_checkstring(L, 1);
    int n = luaL_checkint(L, 2);
    lua_pushinteger(L, set_config(key, &n, sizeof(n)));
}

LUA_CBS_BUILD(lua_reset_config, reset_config, char*, string)

static int lua_sync_config(lua_State *L)
{
    lua_pushinteger(L, sync_config());
    return 1;
}

static const luaL_reg _config_M[] =  {
    {"get",         lua_get_config},
    {"get_int",     lua_get_config_int},
    {"set",         lua_set_config},
    {"set_int",     lua_set_config_int},
    {"reset",       lua_reset_config},
    {"sync",        lua_sync_config},
    {NULL,      NULL},
};

static void hpio_open_config(lua_State *L)
{
    luaL_newmetatable(L, HPIO_CONFIG);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _config_M);
    lua_setfield(L, -2, "config");
}


static int lua_mdir_open(lua_State *L)
{
    char *path = luaL_checkstring(L, 1);
    int n = luaL_checkint(L, 2);
    int sub = luaL_checkint(L, 3);
    char filter[100] = "";
    switch(n) {
        case 1: {
            get_config(CFG_APP_SUPPORT_EXT_MUSIC, filter, 100);
            break;
        }
        case 2: {
            get_config(CFG_APP_SUPPORT_EXT_VIDEO, filter, 100);
            break;
        }
        case 3: {
            get_config(CFG_APP_SUPPORT_EXT_PHOTO, filter, 100);
            break;
        }
        case 4: {
            get_config(CFG_APP_SUPPORT_EXT_RECORDER, filter, 100);
            break;
        }
        default: {
            get_config(CFG_APP_SUPPORT_EXT_ALL, filter, 100);
            break;
        }
    }
    lua_pushinteger(L, media_dir_open(path, filter, (sub != 0) , 0));
    return 1;
}

LUA_CBS_BUILD(lua_mdir_close, media_dir_list_close, void*, int)

static int lua_mdir_filter(lua_State *L)
{
    void *mlist = luaL_checkint(L, 1);
    char *dir = luaL_checkstring(L, 2);
    lua_pushinteger(L, media_dir_filter_dir(mlist, dir));
    return 1;
}

int lua_id3_info(lua_State *L)
{
    char *url = luaL_checkstring(L, 1);
    
    mp_id3_t id3_info;
    int msec = 0;
    memset(&id3_info, 0, sizeof(id3_info));
    mp_get_info_init();
    mp_get_id3_from_uri(&id3_info, &msec, url);
    mp_get_info_end();

    lua_newtable(L);
    PUSH_MEMBER_STR_TAB(L, id3_info.author);
    PUSH_MEMBER_STR_TAB(L, id3_info.composer);
    PUSH_MEMBER_STR_TAB(L, id3_info.album);
    if(*id3_info.title) {
        PUSH_MEMBER_STR_TAB(L, id3_info.title);
    } else {
        char title[128] = "";
        get_music_name(url, title, 128);
        PUSH_MEMBER_STR_TAB(L, title);
    }
    return 1;

}

static int lua_mdir_item(lua_State *L)
{
    void *mlist = luaL_checkint(L, 1);
    int total = 0;
    int finish = 0;
    int dir = 0;
    media_get_item_num(mlist, &total, &dir, &finish);
    lua_newtable(L);
    PUSH_MEMBER_TAB(L, total);
    PUSH_MEMBER_TAB(L, finish);
    PUSH_MEMBER_TAB(L, dir);
    return 1;
}

static const luaL_reg _mlist_M[] = {
	{"open",        lua_mdir_open},
	{"close",       lua_mdir_close},
	{"get_item",    lua_mdir_item},
	{"id3_info",    lua_id3_info},
	{NULL,			NULL}
};
static void hpio_open_mlist(lua_State *L)
{
    luaL_newmetatable(L, HPIO_MLIST);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _mlist_M);
    lua_setfield(L, -2, "mlist");
}


static int lua_screen_off(lua_State *L)
{
    int off = luaL_checkint(L, 1);
    int ret = 0;
    if(off) {
        ret = systime_enter_screensaver();
    } else {
        ret = systime_exit_screensaver();
    }
    lua_pushvalue(L, ret);
    return 1;
}
static const luaL_reg _screen_M[] = {
	{"off",          lua_screen_off},
	{NULL,			NULL}
};
static void hpio_open_screen(lua_State *L)
{
    luaL_newmetatable(L, HPIO_MLIST);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _screen_M);
    lua_setfield(L, -2, "screen");
}

static int build_list(lua_State *L)
{
    format_ign_list();
    return 0;
}
static int get_file_index(lua_State *L) 
{
    char *uri = luaL_checkstring(L, 1);
    char suffix[6] = "";
    int index = sep_to_index_suffix(uri, suffix);
    lua_pushinteger(L, index);
    lua_pushstring(L, suffix);
    return 2;
}
static const luaL_reg _ign_M[] = {
    {"build",       build_list},
    {"get",         get_file_index},
	{NULL,			NULL}
};
static void hpio_open_ign(lua_State *L)
{
    luaL_newmetatable(L, HPIO_MLIST);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, _ign_M);
    lua_setfield(L, -2, "igc");
}

static int _chinese_to_num_force(lua_State *L) 
{
    char *str = luaL_checkstring(L, 1);
    lua_pushinteger(L, chinese_num(str, 1));
    return 1;
}

static int _chinese_to_num(lua_State *L) 
{
    char *str = luaL_checkstring(L, 1);
    lua_pushinteger(L, chinese_num(str, 0));
    lua_pushstring(L, str_befor_ch_num(str));
    return 2;
}

static const luaL_reg ch_num[] = {
    {"convert_force",   _chinese_to_num_force},
    {"convert",         _chinese_to_num},
	{NULL,			NULL}
};
static void hpio_open_chnum(lua_State *L)
{
    luaL_newmetatable(L, HPIO_MLIST);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "_index");
    luaL_register(L, NULL, ch_num);
    lua_setfield(L, -2, "ch_num");
}

int luaopen_hpio(lua_State *L)
{
    luaL_newmetatable(L, "hpio_root");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    // lua_pushvalue(L, -1);

    hpio_open_msg(L);
    hpio_open_player(L);
    hpio_open_wifi(L);
    hpio_open_config(L);
    hpio_open_mlist(L);
    hpio_open_screen(L);
    hpio_open_chnum(L);
    lua_setglobal(L, "hpio");
    return 0;
}

void hpio_start()
{
    // _L = lua_open();
    // luaL_openlibs(_L);
    // luaopen_hpio(_L);
    // return 0;
}

void hpio_stop()
{
    // lua_close(_L);
    // return 0;
}

