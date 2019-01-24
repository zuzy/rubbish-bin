#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <case_msg.h>
#include <syslog.h>

#include "applib.h"
#include "act_debug.h"
#include "media_navi_api.h"
#include "smart_player_api.h"
#include "smart_player_hal.h"
#include "net/wifi_manager.h"
#include "msg_server_api.h"
#include "music_player_api.h"



static int _proc_srv_msg(srv_msg_t *msg, srv_msg_t *reply){
    switch(msg->type){
        case HOPE_PLAYER_STATUS_UPDATE:{
            // print_zizy("get hope player update");
            _status_update((splayer_hal_update_t*)msg->content);
            break;
        }
        case HOPE_DEV_UNBIND:{
            reply->c_size = 0;
            if(hope_unbind() >= 0){
                reply->type = MSG_REPLY_SUCCESS;
            } else {
                reply->type = MSG_REPLY_FAILED;
            }
            break;
        }
        default:break;
    }
    return 0;
}
static first = 0;
static int _proc_sys_msg(msg_apps_t *msg){
    static int sock_status = 0;
    int result = 0;
    // print_zizy("!!!!!!!!!!msg get type %d", msg->type);
    switch(msg->type){
        case MSG_APP_QUIT:{
            print_zizy("!!! exit msg loop !!!");
            exit_msg_loop();
            s_flag |= BIT_EXIT_NETD;
            break;
        }
        // case MSG_CARD_OUT:
        // case MSG_CARD_IN:
        // case MSG_USB_HOST_DISK_IN:
        // case MSG_USB_HOST_DISK_OUT:
        // case MSG_USB_B_OUT:
        // case MSG_USB_B_IN:{
        //     // if(wifi_status){
        //     //     // sleep(1);
        //     //     int mdisk = 0;
        //     //     get_config(CFG_MAIN_DISK, &mdisk, sizeof(int));
        //     //     if(_main_disk != mdisk){
        //     //         _main_disk = mdisk;
        //     //         get_tiems();
        //     //     }
        //     // }
        //     // syslog(LOG_NOTICE,"=====>get service msg, type %d\n",msg->type);
        //     break;
        // }
        case MSG_WIFI_STATE_CHANGED:{
            wifi_info_t info;
			memcpy(&info,(wifi_info_t*)msg->content,sizeof(wifi_info_t));
			print_zizy("MSG_WIFI_STATE_CHANGED,info.status = %d",info.status);
            if(info.status == WIFI_STATE_CONNECTED){
                wifi_status = 1;
            }else{
                wifi_status = 0;
            }
            break;
        }
        case MSG_HOPE_STATUS_UPDATE:{
            // _status_update((splayer_hal_update_t*)msg->content);
            break;
        }
        case MSG_SPLAYER_MODE_CHANGE:{
            splayer_status_t status;
            splayer_get_status(&status);
            int list_mode = 0;
            get_config(CFG_MUSIC_LIST_MODE, &list_mode, sizeof(list_mode));
            syslog(LOG_INFO, "[SPLAYER_MODE_CHANGED]: >>>>>> mode:%d\tl_mode%d\n", status.mode, list_mode);
            if(SPLAYER_MODE_PLAYLIST == status.mode && wifi_status != 0){
                get_config(CFG_MEDIALIST_FD, &mlist_fd, sizeof(void*));
                if(mlist_fd == NULL){
                    print_zizy("media fd is null!");
                    break;
                }
                media_list_add_owner(mlist_fd);
                print_zizy("mlist is %d", mlist_fd);
                update_music_list();
            }
            break;
        }
        case MSG_HOPE_UNBIND: {
            hope_unbind();
            break;
        }

        // case MSG_MEDIA_NAVI_UPDATE:{
        //     // if(wifi_status == 0){
        //     //     break;
        //     // }
        //     // int total, dirnum, finish, total_items, msec;
        //     // mp_id3_t id3_info;
        //     // int mode = 0;
        //     // set_config(CFG_MUSIC_LIST_MODE, &mode, sizeof(mode));
        //     // sync_config();
        //     // void *ffd = NULL;
        //     // memcpy(&ffd, msg->content, sizeof(void *));
        //     // print_zizy("MSG_MEDIA_NAVI_UPDATE (%d:%d)", mlist_fd, ffd);
        //     // mlist_fd = ffd;
        //     // media_get_item_num(mlist_fd, &total, &dirnum, &finish);
        //     // print_zizy("total %d, dirnum %d, finish %d", total, dirnum, finish);
        //     // if(finish && total > 0){
        //     //     update_music_list();
        //     // }
        //     break;
        // }
        // case MSG_TOUCH_INPUT:{
        //     input_msg_t input_msg;
        //     memcpy(&input_msg, msg->content, sizeof(input_msg_t));
        //     _callback(&input_msg);
		// 	g_scene_playing->keydown = false;
        // }
        // case MSG_TP_LONG_DOUBLE:{
        //     tp_result_t result;
        //     print_zizy("memcpy");
        //     memcpy(&result, msg->content, sizeof(tp_result_t));
        //     print_zizy("get tp result %d x:%d y:%d", result.type, result.x, result.y);
        //     break;
        // }
        default:{

            break;
        }
    }
    return 0;
}
#endif

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
    wifi_status = 0;
    ori_volume = 0;
    
    first = 1;
    print_zizy("halo lan manager\n");
    openlog(argv[0], LOG_PID|LOG_NDELAY|LOG_NOWAIT, LOG_DAEMON);
    int i;
    int result = true;
    msg_apps_t msg;
    result = applib_init(argc, argv);
    if( false == result )
    {
        print_err("_init error!");
        goto INIT_ERR;
    }
	signal(SIGINT, _exit_handle);
	signal(SIGPIPE, SIG_IGN);
	
#if 1
    const char *_p_model = "HOPE_ENTERTAINMENT_ATALK_HOPE_Q3";
    set_config("ALI_AI_PRODUCT_MODEL", _p_model, 81);
    const char *_p_key = "V0Gw5DvgK0ppIxrbJF4E";
    set_config("ALI_AI_PRODUCT_KEY", _p_key, 21);
    const char *_p_secret = "VDfGaYnFPFRRCXXkTkNQy5otqzbXypTDlsDeZOUM";
    set_config("ALI_AI_PRODUCT_SECRET", _p_secret, 41);
    const char *sn = "74001805110012";
    set_config("ALI_AI_SN", sn, strlen(sn)+1);
    sync_config();
#endif

    media_navi_open();
	
    register_srv_dispatcher((srv_msg_proc)_proc_srv_msg);
    register_sys_dispatcher((sys_msg_proc)_proc_sys_msg);
    wifi_status_e status;
	hotplug_get_wifi_status(&status);
    if (status == WIFI_STATE_CONNECTED) {
        wifi_status = 1;
    } else {
        wifi_status = 0;
    }
    sleep(5);
    start_485_server();
    pthread_mutex_init(&hlock, NULL);
    start_halo_multicast();
    start_halo_server();
    cloud_client_start();
    while(true == get_msg(&msg))
    {
        dispatch_msg(&msg);
    }
    pthread_mutex_destroy(&hlock);
    unregister_sys_dispatcher();
    unregister_srv_dispatcher();

    if(mlist_fd != NULL){
        media_dir_list_close(mlist_fd);
    }
    closelog();
    media_navi_close();
    applib_quit();

INIT_ERR:
	closelog();
	
    return 0;
}
