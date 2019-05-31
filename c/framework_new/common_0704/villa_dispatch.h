#ifndef __VILLA_DISPATCH_H__
#define __VILLA_DISPATCH_H__

#include "common.h"

#define VILLA_DATA_LEN  2048

#pragma pack(1)
static struct _st_villa_data {
    uint8_t     guid;
    uint16_t    length;
    uint16_t    cmd;
    uint8_t     data[VILLA_DATA_LEN];
    uint8_t     chk;
} villa_data;
#pragma pack()

// #define MAX_
struct _st_villa_pro
{
    uint16_t cmd;
    char name[MAX_PNAME_LEN];
    callback discb;
};

#define REQ_HEART   "REQUEST_HEART_BEAT"
#define REQ_DEV     "REQUEST_DEVICE"
#define REQ_SCE     "REQUEST_SCE"
#define REQ_RM      "REQUEST_ROOM"
#define REQ_FLR     "REQUEST_FLOOR"
#define REQ_ATTR    "REQUEST_ATTRIBUTE"
#define REQ_CTL     "REQUEST_CONTROL"
#define REQ_M_CTRL  "REQUEST_MUSIC_CONTROL"
#define REQ_M_SPE   "REQUEST_SPECIFY_PLAY"
#define REQ_M_TUN   "REQUEST_OPER_TUNNEL"
#define REQ_M_INFO  "REQUEST_INFO"
#define REQ_M_LIST  "REQUEST_GET_SONGLIST"

#define RE_HEART   "RESPONSE_HEART_BEAT"
#define RE_DEV     "RESPONSE_DEVICE"
#define RE_SCE     "RESPONSE_SCE"
#define RE_RM      "RESPONSE_ROOM"
#define RE_FLR     "RESPONSE_FLOOR"
#define RE_ATTR    "RESPONSE_ATTRIBUTE"
#define RE_CTL     "RESPONSE_CONTROL"
#define RE_M_CTRL  "RESPONSE_MUSIC_CONTROL"
#define RE_M_SPE   "RESPONSE_SPECIFY_PLAY"
#define RE_M_TUN   "RESPONSE_OPER_TUNNEL"
#define RE_M_INFO  "RESPONSE_INFO"
#define RE_M_LIST  "RESPONSE_GET_SONGLIST"


enum {
    VILLA_NOP = 0X00,
    // heart beat
    VILLA_HEART = 0X01,
    VILLA_HEART_RE,
    // control
    VILLA_CTRL,
    VILLA_CTRL_RE,
    // device list info
    VILLA_DLIST,
    VILLA_DLIST_RE,
    // scene list info
    VILLA_SLIST,
    VILLA_SLIST_RE,
    // room list info
    VILLA_RLIST,
    VILLA_RLIST_RE,
    // floor list info
    VILLA_FLIST,
    VILLA_FLIST_RE,
    // artribute list info
    VILLA_ALIST,
    VILLA_ALIST_RE,
    // assist ctrl almost about music
    VILLA_ASSIST,
    VILLA_ASSIST_RE,
};
int villa_parse(void *arg);

#endif