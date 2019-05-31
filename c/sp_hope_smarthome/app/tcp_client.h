#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>

#define HALO_TCP_CLI_DEBUG 0
#if HALO_TCP_CLI_DEBUG == 1
    #define print_cli(format, arg...)   do { printf("\033[31m[halo_cli]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_cli(format, arg...)   NULL
#endif

enum{
    C_DEV_COMMON = 0X01,
    C_DEV_HEART,
    C_DEV_EXIT,
    C_DEV_IDENTIFY,
    C_DEV_CTRL = 0X10,
    C_DEV_SONGLIST_NEW = 0X20,
    C_DEV_SONGLIST_CREATE = 0X30,
    C_DEV_SONGLIST_RM = 0X40,
    C_DEV_SONGLIST_ADD = 0X50,
    C_DEV_AREA_SCAN = 0X60,
    C_DEV_AREA_CTRL = 0X70,
    C_DEV_SONGLIST_RMSONG = 0X80,
    C_DEV_SCENE_NEW = 0X90,
    C_DEV_SCENE_RULE = 0X100,
    C_DEV_SONGLIST_CHANGE = 0X125,

    C_DEV_SCENE_CTRL = 0X170,
    C_DEV_SCENE_START= 0X175,
    C_DEV_SCENE_RM = 0X185,
    C_DEV_AREA_VOL = 0X195,

    C_DEV_UNBIND = 0X215,
    
    C_DEV_EVENT = 0X230,

    C_DEV_TTS = 0X250,
    C_DEV_TTS_CHECK = 0X260,
};
int tcp_connect(int cli_fd, struct sockaddr_in server_addr, int isblock);
int add_tcp_cli();
int tcp_cli_update_status(void *arg);
int hope_update_status(int force);
char *_build_payload(uint16_t cmd, char *data, int *len);
int hope_unbind();
#endif