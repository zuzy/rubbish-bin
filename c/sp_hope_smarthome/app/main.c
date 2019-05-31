#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include<fcntl.h>
#include<termios.h>
#include <syslog.h>
#include "net/config.h"

#include "msg_server_api.h"
#include "music_player_api.h"
#include "smart_player_api.h"

#include "net/wifi_manager.h"
#include "list.h"
#include "common.h"

#include "cJSON.h"

#include "main_loop.h"
#include "main_pipe.h"
#include "tcp_client.h"
#include "udp_ser.h"
#include "tcp_ser.h"
#include "halo_http_regist.h"
#include "halo_http_mlist.h"
#include "hope_msg.h"
#include "serial_rs485.h"
#include "villakit.h"
#include "hp_speech.h"
#include "halo_hal.h"
#include "lua_common.h"

#define BIT_INIT_NET                (0x1 << 0)
#define BIT_EXIT_NETD               (0x1 << 1)
#define UPDATE_TIME                 (5 << 10)
static unsigned int s_flag = 0;
static int mlist_timer = -1;
static int mlist_retry = 0;

static int _get_items(void *arg)
{
    if(get_items() == 0) {
        if(++mlist_retry >= 3) {
            kill_timer(mlist_timer);
            mlist_timer = -1;
        }
        print_main("open mlist ok!!");
        // modify_timer(mlist_timer, UPDATE_TIME);
        // mlist_timer = -1;
        return 0;
    }
    print_main("open mlist error!");
    return 1;
}

static int _abs(int a) {
    return a < 0 ? -a : a;
}

static int _statuscmp(splayer_status_t *des, splayer_status_t *src)
{
    REQUIRE_NOLOG((des->mode != src->mode), Ret);
    REQUIRE_NOLOG((des->status != src->status), Ret);
    REQUIRE_NOLOG((des->loop_mode != src->loop_mode), Ret);
    REQUIRE_NOLOG((des->volume != src->volume), Ret);
    REQUIRE_NOLOG((des->playlist_index != src->playlist_index), Ret);
    REQUIRE_NOLOG((_abs(des->current_time - src->current_time) > 1000), Ret);
    return 0;
Ret:
    return 1;
}

static int _player_status(void *arg)
{
#if 1
    splayer_status_t status;
    static splayer_status_t _s_status;
    REQUIRE((splayer_get_status(&status)), Error);
    int list_mode = 0;

    if(_statuscmp(&status, &_s_status)) {
        print_main("status changed!!");
        memcpy(&_s_status, &status, sizeof(status));

        // print_common("mode %d, index %d", status.mode, *halo_handle->list_index);
        #if 0
        if(status.mode == SPLAYER_MODE_LOCAL && halo_handle->local_list != NULL && status.status == SPLAYER_STATUS_STOP) {
            ++halo_handle->list_index;
            print_common("uri list one song ok %d", *halo_handle->list_index);
            if(*halo_handle->list_index >= 0) {
                halo_local_index(*halo_handle->list_index);
            } else {
                splayer_set_mode(SPLAYER_MODE_NULL);
                free(halo_handle->local_list);
                halo_handle->local_list = halo_handle->list_index = NULL;
            }
            return 0;
        }
        #endif
        #if 0
        if(status.mode == SPLAYER_MODE_PLAYLIST) {
            get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(int));
            if(list_mode == 0) {
                update_pipe(&status, sizeof(status));
            }
        }
        #else
        if(status.mode == SPLAYER_MODE_PLAYLIST) {
            get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(int));
            if(list_mode == 0) {
                update_pipe(&status, sizeof(status));
            }
        } else if(status.mode == SPLAYER_MODE_AIMUSIC || status.mode == SPLAYER_MODE_CLOUDMUSIC) {
            update_pipe(&status, sizeof(status));
        }
        #endif
    }
    _s_status.current_time = status.current_time;
#else

#endif
    return 0;
Error:
    return -1;
}

#if 0
int main(int argc, char *argv[])
{
    halo_init(NULL);
    add_pipe();
    add_tcp_cli();
    add_tcp_ser();
    udp_service_init();
    add_http_regist();
    halo_start();
    // wifi_connected = 1;
    // alink_connect = 1;
    while(1){
         // sleep(5);
        sleep(1);
        // update_pipe(NULL);
        // wifi_connected = !wifi_connected;
        // alink_connect = !alink_connect;
        // print_main("%d", wifi_connected);
    }
    return 0;
}
#else

static int _proc_srv_msg(srv_msg_t *msg, srv_msg_t *reply)
{
    switch(msg->type) {
        case HOPE_DEV_UNBIND: {
            set_config(CFG_HREG_MAC, "ffff", 32);
            // sync_config();
            reply->c_size = 0;
            if(hope_unbind() >= 0) {
                reply->type = MSG_REPLY_SUCCESS;
            } else {
                reply->type = MSG_REPLY_FAILED;
            }
            break;
        }
        case HOPE_TP_DEV_CTRL:{
            int id, status;
            memcpy(&id, msg->content, sizeof(int));
            memcpy(&status, msg->content + sizeof(int), sizeof(int));
            print_zizy("get id: %d %d", id, status);
            int ret = smart_home_tp_dev_ctrl(id , status);
            reply->type = MSG_REPLY_SUCCESS;
            reply->c_size = sizeof(int);
            memcpy(reply->content, &ret, sizeof(int));
            break;
        }
        // case HOPE_GET_SPEECH: {
        //     print_speech("get cmd %d %s", msg->type, msg->content);
        //     parse_speech_cmd(msg->content);
        //     break;
        // }
        default:break;
    }
    return 0;
}

static int _proc_sys_msg(msg_apps_t *msg)
{
    static int sock_status = 0;
    int result = 0;
    static int play_mode = -1;
    switch(msg->type) {
        case MSG_APP_QUIT: {
            print_main("!!! exit msg loop !!!");
            exit_msg_loop();
            s_flag |= BIT_EXIT_NETD;
            break;
        }
        case MSG_SAIR_CLOUD_CUSTOM_COMMAND: {
            print_speech("get speech : %s", msg->content);
            // hope_speech_cmd(msg->content);
            parse_speech_cmd(msg->content);
            break;
            // speech cmd of asr!
        }
        case MSG_CARD_OUT:
        case MSG_CARD_IN:
        case MSG_USB_HOST_DISK_IN:
        case MSG_USB_HOST_DISK_OUT:
        case MSG_USB_B_OUT:
        case MSG_USB_B_IN: {
            
            // break;

            wifi_status_e status;
            hotplug_get_wifi_status(&status);
            if (status == WIFI_STATE_CONNECTED) {
                // sleep(1);
                int mdisk = 0;
                get_config(CFG_MAIN_DISK, &mdisk, sizeof(int));
                if(halo_handle->main_disk != mdisk) {
                    halo_handle->main_disk = mdisk;
                    // get_items();
                    if(mlist_timer >= 0) {
                        kill_timer(mlist_timer);
                    }
                    mlist_retry = 0;
                    mlist_timer = set_timer(5000, _get_items, NULL);
                }
            }
            break;
        }
        case MSG_SPLAYER_MODE_CHANGE:{
            // break;
            
            // reset the list array
            free(halo_handle->local_list);
            halo_handle->local_list = halo_handle->list_index = NULL;
            
            splayer_status_t status;
            splayer_get_status(&status);
            #if 1
            if(play_mode != SPLAYER_MODE_AIMUSIC && play_mode != SPLAYER_MODE_CLOUDMUSIC) {
                play_mode = status.mode;
                break;
            }
            play_mode = status.mode;
            #endif
            int list_mode = 1;
            get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(list_mode));
            // syslog(LOG_INFO, "[SPLAYER_MODE_CHANGED]: >>>>>> mode:%d\tl_mode%d\n", status.mode, list_mode);
            if(SPLAYER_MODE_PLAYLIST == status.mode && list_mode == 0){
                #if 1
                get_config(CFG_MEDIALIST_FD, &halo_handle->mlist, sizeof(void*));
                if(halo_handle->mlist == NULL){
                    print_main("media fd is null!");
                    break;
                }
                media_list_add_owner(halo_handle->mlist);
                print_main("mlist is %d", halo_handle->mlist);
                mlist_update(halo_handle->mlist);
                #endif
            }
            break;
        }
        case MSG_MEDIA_NAVI_UPDATE: {

            // break;

            int dirnum, finish, total_items, msec;
            media_get_item_num(halo_handle->mlist, &halo_handle->music_num, &dirnum, &finish);
            print_main("get mlist <%ld> %d %d", halo_handle->mlist, halo_handle->music_num, finish);
            if(finish) {
                mlist_update(halo_handle->mlist);
            }
            break;
        }
        case MSG_LUA_DEMO: {
            printf("\n--------------\n lua_demo_broad_cast\n%s\n----------------\n", msg->content);
            break;
        }

        default:{

            break;
        }
    }
    return 0;
}

static void _exit_handle(int sig)
{
	if(sig == SIGINT) {
		syslog(LOG_INFO, "halotest exit by signal INT\n");
		exit_msg_loop();
        s_flag |= BIT_EXIT_NETD;
	}
}


int main(int argc, char *argv[])
{
    openlog(argv[0], LOG_PID|LOG_NDELAY|LOG_NOWAIT, LOG_DAEMON);
    int i;
    int result = true;
    msg_apps_t msg;
    int status_timer;

    halo_init(NULL);
    add_pipe();
    add_serial();
    udp_service_init();
    add_tcp_cli();
    add_tcp_ser();
    add_http_regist();
    add_villa_pro();
    halo_start();
    
    hpio_start();
    applib_init(argc, argv);

	signal(SIGINT, _exit_handle);
	signal(SIGPIPE, SIG_IGN);

    #if 1
    media_navi_open();
    
	status_timer = set_timer(500, _player_status, NULL);
    register_srv_dispatcher((srv_msg_proc)_proc_srv_msg);
    register_sys_dispatcher((sys_msg_proc)_proc_sys_msg);

    while(true == get_msg(&msg))
    {
        dispatch_msg(&msg);
    }

    unregister_sys_dispatcher();
    #else
    int rrr = 0;
    while(1) {
        sleep(2);
        print_common("\n\n\nnotice start \n\n\n");
        lua_call_fun("/tmp/lua/timer_wifi.lua", "get_wifi_status", "s>i", "helo", &rrr);
        hpio_call("/tmp/lua/timer_wifi.lua", "get_wifi_status", "s>i", "helo", &rrr);
        print_common("get ret %d", rrr);
    }
    #endif

    if(halo_handle->mlist != NULL){
        media_dir_list_close(halo_handle->mlist);
    }
    hpio_stop();
    closelog();
    media_navi_close();
    applib_quit();

INIT_ERR:
	closelog();
	
    return 0;
}
#endif