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

#include <applib.h>
#include "act_debug.h"
#include "media_navi_api.h"
#include "music_player_api.h"
// #include "applib/app_config.h"

#include "get_music_list.h"
#define CFG_MAX_BUFF_LENGTH 100

void *mlist_fd;
media_item_t *items;
char path[1024];
int get_tiems(){
    char filter[50];
    char dir_filter[50];    
    char play_path[50];
    strcpy(play_path, "/mnt/uhost/");
    // sprintf(filter, "mp3|wav|");
    get_config(CFG_APP_SUPPORT_EXT_MUSIC, filter, CFG_MAX_BUFF_LENGTH);
    print_zyf("filter %s", filter);

    print_zyf("media navi open");
    mlist_fd = media_dir_open(play_path, filter, 1, 0);

    // int nofile = 0,update = 0,hasindex = 0;
    // int total, dirnum, finish;
    // int index, count, total_items, i;
    // media_item_t *items;

    // print_zyf("media_fd=0x%x",mlist_fd);
    // media_get_item_num(mlist_fd, &total, &dirnum, &finish);
    // print_zyf("total %d, dirnum %d, finish %d", total, dirname, finish);
    // total_items = media_get_items(mlist_fd, 0, MLISTMAXNUM, items);
    // mp_get_cur_url(path, 1024);
    // while(total_items){
    //     print_zyf("%d, item:%s", total_items, items->url);
    //     --total_items;
    //     ++items;
    // }
    // print_zyf("path is %s", path);
    // media_navi_close();
    return 0;
}