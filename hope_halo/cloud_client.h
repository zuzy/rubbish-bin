#ifndef __CLOUD_CLIENT_H__
#define __CLOUD_CLIENT_H__

#include "http_regist.h"

#define CLOUD_HOST_NAME     "192.168.2.9"
#define CLOUD_HOST_PORT     8888

#pragma pack(1)
typedef struct{
    uint16_t  cmd;
    uint8_t   type;
    uint16_t  len;
    uint8_t   snd_id[10];
}cloud_head_t;
#pragma pack()

/*
enum{
    C_DEV_COMMON_REPLY = 0X01,          // 设备端通用应答
    C_DEV_HEARTBEAT,                    // 设备端心跳
    C_DEV_EXIT,                         // 设备端下线
    C_DEV_IDENTIFY,                     // 设备端认证
    C_DEV_OPERATION = 0X10,             // 设备端操作控制
    C_DEV_SONGLIST_NEW = 0X20,          // 设备端 新建 修改 歌单
    C_DEV_SONGLIST_CREATE = 0X30,       // 设备端 新建歌单加入歌曲信息
    C_DEV_SONGLiST_RM = 0X40,           // 设备端 删除歌单
    C_DEV_SONGLIST_ADDSONG = 0X50,      // 设备端 加入歌曲到歌单
    C_DEV_AREA_COMMON_REPLY = 0X60,     // 设备端 回复分区查询
    C_DEV_AREA_CTRL_REPLY = 0X70,       // 设备端 设置/回复分区设置
    C_DEV_SONGLIST_RMSONG = 0X80,       // 设备端 从歌单删除歌曲
    C_DEV_SCENE_NEW = 0X90,             // 设备端 新建/修改情景
    C_DEV_SCENE_RM = 0X0100,            // 设备端 删除情景
    C_DEV_SONGLIST_CHANGE = 0X0125,     // 设备端 歌曲列表切换
    C_DEV_SCENE_CTRL = 0X0170,          // 设备端 开启/关闭情景
    C_DEV_SCENE_START = 0X0175,         // 设备端 执行情景
    C_DEV_SCENE_CTRL_ALL = 0X0185,      // 设备端 开启/关闭所有情景
    C_DEV_AREA_VOL = 0X0195,            // 设备端 设置/回复分区音量设置

    C_SER_COMMON_REPLY = 0X8001,        // 服务端 通用应答
    C_SER_OPERATION = 0X8005,           // 服务端 播放控制
    C_SER_SONGLIST_NEW = 0X8015,        // 服务端 新建/修改歌单
    C_SER_SONGLIST_CREATE = 0X8025,     // 服务端 新建歌单加入歌曲信息
    C_SER_SONGLIST_RM = 0X8035,         // 服务端 删除歌单
    C_SER_SONGLIST_ADDSONG = 0X8045,    // 服务端 加入歌曲到歌单
    C_SER_AREA_SCAN = 0X8055,           // 服务端 查询所有分区
    C_SER_AREA_CTRL = 0X8065,           // 服务端 设置分区开关
    C_SER_SONGLIST_RMSONG = 0X8075,     // 服务端 从歌单删除歌曲
    C_SER_SCENE_NEW =0X8085,            // 服务端 新增/修改情景
    C_SER_SCENE_RM = 0X8095,            // 服务端 删除情景
    C_SER_SHARE_DEV = 0X8105,           // 服务端 设备分享
    C_SER_NOTIFY_EVENT = 0X8110,        // 服务端 通知事件下发
    C_SER_PUB_MSG = 0X8115,             // 服务端 文本消息推送
    C_SER_CLOSE_CONNECTION = 0X8120,    // 服务端 关闭终端指定链接
    C_SER_COLLECT_CTRL = 0X8130,        // 服务端 收藏/取消收藏
    C_SER_PLAY_SORT = 0X8135,           // 服务端 歌曲按类别批量播放
    C_SER_PLAY = 0X8140,                // 服务端 歌曲批量播放
    C_SER_ADDTO_LATELY = 0X8145,        // 服务端 歌曲列表加入最近歌单
    C_SER_ADDTO_EXIST = 0X8150,         // 服务端 播放列表加入已有歌单
    C_SER_ADDTO_NEW = 0X8155,           // 服务端 收藏播放列表到新歌单
    C_SER_CLEAR_PLAYLIST = 0X8160,      // 服务端 清空当前播放列表
    C_SER_SCENE_CTRL = 0X8165,          // 服务端 开启/关闭情景
    C_SER_SCENE_CTRL_ALL = 0X8180,      // 服务端 开启/关闭所有情景
    C_SER_SCENE_START = 0X8190,         // 服务端 立即执行设备情景模式
};
*/

enum{
    C_CMD_COMMON = 0X01,
    C_CMD_HEART = 0X02,
    C_CMD_CTRL = 0X05,
    C_CMD_SONGLIST_NEW = 0X15,
    C_CMD_SONGLIST_CREATE = 0X25,
    C_CMD_SONGLIST_RM = 0X35,
    C_CMD_SONGLIST_ADD = 0X45,
    C_CMD_AREA_SCAN = 0X55,
    C_CMD_AREA_CTRL = 0X65,
    C_CMD_SONGLIST_RMSONG = 0X75,
    C_CMD_SCENE_NEW = 0X85,
    C_CMD_SCENE_RULE = 0X95,
    C_CMD_SHARE = 0X105,
    C_CMD_EXIT = 0X120,
    C_CMD_COLLECT_CTRL = 0X130,
    C_CMD_PLAY_SONGLIST = 0X135,
    C_CMD_PLAY_SONGS = 0X140,
    C_CMD_ADDTO_CUR = 0X145,
    C_CMD_ADDTO_ELSE = 0X150,
    C_CMD_ADDTO_NEW = 0X155,
    C_CMD_CLEAR_SONGLIST = 0X160,
    C_CMD_SCENE_CTRL = 0X165,
    C_CMD_SCENE_RM = 0X180,
    C_CMD_SCENE_START= 0X190,
    C_CMD_GET_STATUS = 0X205,
    C_CMD_SCENE_CTRLALL = 0X220,
    C_CMD_SCENE_RMALL = 0X225,
};



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
    C_DEV_EVENT = 0X230,

};

enum{
    C_STATUS_SUCCESS = 50000,
    C_STATUS_TOO_SHORT,
    C_STATUS_FORM_NO_MARK,
    C_STATUS_FORM_ERR_MARK,
    C_STATUS_ERR_CHECKSUM,
    C_STATUS_BODY_TOO_SHORT,        //5
    C_STATUS_CMD_ERR,
    C_STATUS_REF_ERR,
    C_STATUS_LEN_ERR,
    C_STATUS_ID_FORM,
    C_STATUS_ID_CAL,                //10
    C_STATUS_ID_EXIST,
    C_STATUS_ID_TYPE,
    C_STATUS_ID_NOT_FOUND,
    C_STATUS_LOSE_CONNECTION,
    C_STATUS_NO_BANDING,            //15
    C_STATUS_SONGLIST,
    C_STATUS_DEV_EXIT, 
    C_STATUS_MSG_FORM,
    C_STATUS_MOB_EXIT,
};

int cloud_client_start(void);
int cloud_client_close(void);

#endif