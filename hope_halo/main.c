#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <case_msg.h>

#include "applib.h"
#include "act_debug.h"
#include "media_navi_api.h"


#include "common.h"
#include "tcp_server.h"
#include "multicast.h"
#include "wifi_api.h"
#include "get_music_list.h"
#include "http_regist.h"

#define MLISTMAXNUM		100

#define BIT_INIT_NET                 (0x1 << 0)
#define BIT_EXIT_NETD                (0x1 << 1)
static unsigned int s_flag = 0;
pthread_t tid[2];

extern volatile int send_status;
extern volatile char g_send_buff[MAXDATASIZE];
extern void *mlist_fd;

// static tp_ld_t tp_ld;

#if 1
// static void _callback(input_msg_t *pmsg){
//     tp_event_t tp_event = pmsg->data.tmsg;
//     catch_tp_event(&tp_ld, *tp_event);
// }


static int netd_proc_sys_msg(msg_apps_t *msg){
    static int sock_status = 0;
    int result = 0;
    switch(msg->type){
        case MSG_APP_QUIT:{
            exit_msg_loop();
            s_flag |= BIT_EXIT_NETD;
            break;
        }
        case MSG_WIFI_STATE_CHANGED:{
            wifi_info_t info;
			memcpy(&info,(wifi_info_t*)msg->content,sizeof(wifi_info_t));
			print_zyf("MSG_WIFI_STATE_CHANGED,info.status = %d",info.status);
            switch (info.status){
                case WIFI_STATE_DISABLED: 
                case WIFI_STATE_DISCONNECTED:{
                    pthread_join(tid[0], NULL);
                    pthread_join(tid[1], NULL);
                    sock_status = 0;
                    break;
                }

                case WIFI_STATE_CONNECTED:{
                    if(!sock_status){
                        char *auth = (char*)malloc(100);
                        char *ref = (char*)malloc(100);
                        bzero(ref, 100);
                        bzero(auth, 100);
                        // regist_device(auth, (unsigned char*)ref);
                        cloud_client_start();
                        pthread_create(&tid[1], NULL, halo_udp_proc, NULL);
                        pthread_create(&tid[0], NULL, halo_tcp_proc, NULL);
                        sock_status = 1;
                        get_tiems();
                        free(auth);
                        free(ref);
                    }
                    break;
                }

                default:{
                    break;
                }
            }
        }
        case MSG_SONG_STATUS:{

            break;
        }
        case MSG_HALO_SEND_TCP:{
            int i;
            if(!send_status){
                strcpy(g_send_buff, (char*)&msg->content);
            }else{
                print_zyf("busy!");
            }
            break;            
        }
        case MSG_MEDIA_NAVI_UPDATE:{
            int total, dirnum, finish, total_items;
            char path[128];
            media_item_t *items = malloc(sizeof(media_item_t)*100);
            print_zyf("MSG_MEDIA_NAVI_UPDATE");
            media_get_item_num(mlist_fd, &total, &dirnum, &finish);
            print_zyf("total %d, dirnum %d, finish %d", total, dirname, finish);
            total_items = media_get_items(mlist_fd, 0, MLISTMAXNUM, items);
            while(total_items){
                print_zyf("%d, item:%s", total_items, items->url);
                --total_items;
                ++items;
            }
            // media_navi_close();
            free(items);
            break;
        }
        // case MSG_TOUCH_INPUT:{
        //     input_msg_t input_msg;
        //     memcpy(&input_msg, msg->content, sizeof(input_msg_t));
        //     _callback(&input_msg);
		// 	g_scene_playing->keydown = false;
        // }
        // case MSG_TP_LONG_DOUBLE:{
        //     tp_result_t result;
        //     print_zyf("memcpy");
        //     memcpy(&result, msg->content, sizeof(tp_result_t));
        //     print_zyf("get tp result %d x:%d y:%d", result.type, result.x, result.y);
        //     break;
        // }
        default:{

            break;
        }
    }
    return result;
}
#endif

int main(int argc, char *argv[])
{
    print_zyf("halo lan manager\n");

    media_navi_open();
    int result = true;
    msg_apps_t msg;
    result = applib_init(argc, argv);
    result = _init();
    
    if( false == result )
    {
        print_err("_init error!");
        goto INIT_ERR;
    }
    // pthread_create(&tid[1], NULL, halo_udp_proc, NULL);
    // pthread_create(&tid[0], NULL, halo_tcp_proc, NULL);
    register_sys_dispatcher((sys_msg_proc)netd_proc_sys_msg);

    while(true == get_msg(&msg))
    {
        dispatch_msg(&msg);
    }
    unregister_sys_dispatcher();
    media_navi_close();
INIT_ERR:
    applib_quit();
    return 0;
}


