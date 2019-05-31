#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <case_msg.h>
#include <sys/types.h>
#include <dirent.h>

#include "applib.h"
#include "apconfig.h"
#include "net/wifi_manager.h"
#include "net/config.h"
#include "act_debug.h"
#include "cJSON.h"
#include "common.h"
#include "main_loop.h"

#include "media_navi_api.h"
#include "smart_player_api.h"
#include "smart_player_hal.h"

#include "music_player_api.h"
#include "halo_hal.h"
#include "hope_api.h"
#include "halo_http_mlist.h"

int halo_src_set(int *src)
{
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);

    switch(src[0]) {
        case SRC_LOCALE: {
            break;
        }
        case SRC_BLE: {
            break;
        }
        case SRC_SCENE: {
            break;
        }
        case SRC_ALINK: {
            break;
        }
        default: break;
    }
    Error:
    return -1;
}

int halo_loop_set(int loop)
{
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);
    REQUIRE(status.mode != SPLAYER_MODE_PLAYLIST, Ret);
    switch(loop) {
        case LOOP_REPEAT_ONE: {
            return splayer_set_loop_mode(SPLAYER_LOOP_REPEAT_ONE);
        }
        case LOOP_REPEAT_ALL: {
            return splayer_set_loop_mode(SPLAYER_LOOP_REPEAT_ALL);
        }
        case LOOP_REPEAT_SHUFFLE: {
            return splayer_set_loop_mode(SPLAYER_LOOP_SHUFFLE);
        }
        case LOOP_SEQUENCE: {
            return splayer_set_loop_mode(SPLAYER_LOOP_SEQUENCE);
        }
        default: break;
    }
    Error:
    return -1;
    Ret:
    return 0;
}
#if 1

int hope_to_local_mode(int index)
{
    msg_apps_t msg;
    msg.type = MSG_MUSIC_SCENE_CHANGE;
    msg.content[0] = HOPE_PRO_LOCAL_MUSIC;
    memcpy(msg.content+1, &index, sizeof(index));
    return broadcast_msg(&msg);
}



int hope_music_play_index(int index)
{
    int list_mode = 1;
    // static int64_t time_now = 0;
    // mode willnot change if acted in 200 ms
    // if(get_now_us() - time_now < 200000ll) {
    //     print_hal("index change too fast");
    // }
    // time_now = get_now_us();
    splayer_status_t status;
    get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(list_mode));
    splayer_get_status(&status);


    if(status.mode != SPLAYER_MODE_PLAYLIST || list_mode != 0) {
        return hope_to_local_mode(index);
    }
    #if 0
    if(SPLAYER_STATUS_STOP == status.status) {
        int dirnum, finish;
        media_get_item_num(halo_handle->mlist, &halo_handle->music_num, &dirnum, &finish);
        if(halo_handle->music_num > 0) {
            splayer_set_mode(SPLAYER_MODE_PLAYLIST);
            splayer_set_playlist(halo_handle->mlist);
            splayer_set_loop_mode(SPLAYER_LOOP_REPEAT_ALL);
        }
    }
    print_hal("total %d", halo_handle->music_num);
    if(halo_handle->music_num < 0 || index >= halo_handle->music_num) {
        return get_items();
    }
    #endif
    if(status.playlist_index != index) {
        splayer_set_index(index);
    }
    return splayer_play();
}

int hope_music_play()
{
    int list_mode = 1;
    print_hal("hope music play");
    splayer_status_t status;
    splayer_get_status(&status);
    get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(list_mode));

    if((status.mode != SPLAYER_MODE_PLAYLIST && status.mode != SPLAYER_MODE_AIMUSIC && status.mode != SPLAYER_MODE_CLOUDMUSIC) || list_mode != 0) {
        return hope_to_local_mode(0);
    }

    switch(status.status) {
        case SPLAYER_STATUS_PAUSE: {
            print_hal("resume");
            // get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(list_mode));
            // if(list_mode == 0 && status.mode == SPLAYER_MODE_PLAYLIST)
            return splayer_resume();
            break;
        }
        case SPLAYER_STATUS_STOP: {
            print_hal("stop & start");
            return hope_music_play_index(0);
        }
        default:break;
    }
    return -1;
}

#endif

int halo_seek(int msec)
{
    // msec = msec == 0 ? 1 : msec;
    msec = msec ? : 1;
    return splayer_seek(msec);
}

int halo_vol(int vol, int en_persent)
{
    if(en_persent) {
        return splayer_set_volume(vol * 4 / 10);
    } else {
        return splayer_set_volume(vol);
    }
}

int halo_mute(int ctrl)
{
    static int _vol = 0;
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);

    if(ctrl == 0) {
        if(0 == status.volume){
            _vol = _vol ? _vol : 16;
            return splayer_set_volume(_vol);
        }
    } else {
        _vol = status.volume;
        if(0 != _vol){
            return splayer_set_volume(0);
        }
    }
    return 0;
Error:
    return -1;
}

int halo_vol_ctrl(int ctrl)
{
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);

    int vol;
    if(ctrl) {
        vol = status.volume + 3;
        vol = vol > 40 ? 40 : vol;
        return splayer_set_volume(vol);
    } else {
        vol = status.volume - 3;
        vol = vol < 0 ? 0 : vol;
        return splayer_set_volume(vol);
    }
Error:
    return -1;
}

int halo_cut(int ctrl)
{
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);

    if(ctrl) {
        return splayer_next();
    } else {
        return splayer_prev();
    }
Error:
    return -1;
}

int halo_play(int ctrl)
{
    splayer_status_t status;
    REQUIRE(splayer_get_status(&status) != 0, Error);

    switch(ctrl) {
        case CTRL_PAUSE: {
            if(status.status == SPLAYER_STATUS_PLAYING) {
                return splayer_pause();
            }
            break;
        }
        case CTRL_PLAY: {
            if(status.status != SPLAYER_STATUS_PLAYING) {
                // balabala
                return hope_music_play();
            }
            break;
        }
        case CTRL_STOP: {
            if(status.status != SPLAYER_STATUS_STOP) {
                return splayer_stop();
            }
            break;
        }
        default: break;
    }
Error:
    return -1;
}

int halo_hal(st_cmd cmd)
{
    switch(cmd.cmd) {
        case HALO_STATUS: {
            break;
        }
        case HALO_PLAY_CTRL: {

            break;
        }
        case HALO_CUT_CTRL: {
            if(cmd.content[0]) {

            } else {
                
            }
            break;
        }
        case HALO_VOL_CTRL: {
            if(cmd.content[0]) {

            } else {

            }
            break;
        }
        case HALO_MUTE_CTRL: {
            if(cmd.content[0]) {

            } else {

            }
            break;
        }
        case HALO_LOOP_SET: {
            switch(cmd.content[0]) {
                case 0: break;
            }
            break;
        }
        case HALO_SRC_SET: {
            break;
        }
        case HALO_VOL_SET: {
            break;
        }
        case HALO_SEEK: {
            break;
        }
        default: break;
    }
    return 0;
}

int halo_local_file(char *uri)
{
    splayer_status_t status;
    splayer_get_status(&status);
    if(status.mode != SPLAYER_MODE_LOCAL) {
        splayer_set_mode(SPLAYER_MODE_LOCAL);
    } else {
        splayer_stop();
    }
    
    splayer_set_loop_mode(SPLAYER_LOOP_ONLY_ONE);
    splayer_set_file(uri);
    return splayer_play();
}

int halo_local_index(int index)
{
    char uri[MAX_BODY_LEN] = "";
    if(index < 0) {
        return -1;
    }
    splayer_status_t status;
    splayer_get_status(&status);
    if(status.mode != SPLAYER_MODE_LOCAL) {
        splayer_set_mode(SPLAYER_MODE_LOCAL);
    } else {
        splayer_stop();
    }
    
    
    lua_call_fun("/tmp/lua/mlist.lua", "get_uri_from", "i>s", index, uri);
    splayer_set_loop_mode(SPLAYER_LOOP_ONLY_ONE);
    splayer_set_file(uri);
    return splayer_play();
}

int check_is_alarm()
{
    splayer_status_t status;
    splayer_get_status(&status);
    if(status.mode == SPLAYER_MODE_LOCAL && status.loop_mode == SPLAYER_LOOP_REPEAT_ONE) {
        return 1;
    } else {
        return 0;
    }
}

int play_music_without_list(int index)
{
    int m_disk = 0;
    char filter[100];
    DIR *dir = NULL;
    int ret = -1;
    char suffix[MAX_SUFFIX_LEN] = "";
    char uri[MAX_BODY_LEN] = "";
    get_config(CFG_APP_SUPPORT_EXT_MUSIC, filter, 100);
    get_config(CFG_MAIN_DISK, &m_disk, sizeof(int));
    char path[128];
    switch(m_disk) {
        case HOTPLUGIN_DEV_TYPE_NONE:
        case HOTPLUGIN_DEV_TYPE_LOCAL: {
            snprintf(path, 128, "/mnt/udisk/%s", LOCAL_IGN_DIR);
            break;   
        }
        case HOTPLUGIN_DEV_TYPE_CARD: {
            snprintf(path, 128, "/mnt/sdcard/%s", LOCAL_IGN_DIR);
            break;
        }
        case HOTPLUGIN_DEV_TYPE_UHOST: {
            snprintf(path, 128, "/mnt/external/%s", LOCAL_IGN_DIR);
            break;
        }
        default: goto Error;
    }
    REQUIRE((dir = opendir(path)) == NULL, Error);
    struct dirent *dret = NULL;
    while((dret = readdir(path)) != NULL) {
        if(dret->d_type == DT_REG) {
            ret = sep_to_index_suffix(dret->d_name, suffix);
            if(ret >= 0) {
                // match the suffix of media, match the index to the file name.
                if(strstr(filter, suffix) != NULL && index == ret) {
                    snprintf(uri, MAX_SUFFIX_LEN, "%s/%s", path, dret->d_name);
                    break;
                }
            }
        }
    }
    if(ret >= 0) {
        closedir(dir);
        dir = NULL;
        return halo_local_file(uri);
    }

    Error: {
        if(dir != NULL) {
            closedir(dir);
            dir = NULL;
        }
        return -1;
    }
}