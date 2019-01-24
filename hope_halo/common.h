#ifndef __HALO_COMMON_H__
#define __HALO_COMMON_H__

#define PORT 19900
#define MAXDATASIZE 1024
#define BACKLOG 5
#define HEARTBEAT 60*3

#define HALO_VERSION    102
#define VERSION_CODE    1060302
#define VERSION_NAME    "1.6.3.2"
#define MODEL           "HOPE-Q3"
#define PLAYER_TYPE     2

// SN:6299201799  deviceCata:Q3 deviceName:HOPE-Q3
// comName: HOPE
// playType:_test

#define boolean_t unsigned char

#define ITEM_CMD        "cmd"
#define ITEM_PARAM      "params"

#define CMD_DISCOVER    "hopediscover"
#define CMD_INFO        "info"
#define CMD_CONTROL     "control"
#define CMD_GETSONGLIST "getsonglist"
#define CMD_GETPLAYLIST "getplaylist"
#define CMD_QUERYSONINFO    "querysonginfo"
#define CMD_OPERLIST    "operlist"
#define CMD_EXIT        "exit"
#define CMD_OPERTUNNEL  "opertunnel"
#define CMD_DEVICEINFO  "deviceinfo"


#define R_COMPANY_NAME    "HOPE"
#define R_DEVICE_SN       "6299201799"
#define R_DEVICE_NAME     "HOPE-Q3"
#define R_DEVICE_CATA     "Q3"
#define R_FIRM_VERSION    "ats3605-debug-0.1"
#define R_SOFT_VERSION    "0.0.1"
#define R_PLAYER_TYPE     "_test"
#define R_PLAYER_VERSION  "1.0"
#define R_BACKGROUND_TYPE "master"
#define R_BACKGROUND_VER  "1.2.3"
#define R_UPGRADE_CHANNEL "release-HOPE-Q3"
#define R_DEVICE_DRIVE    "ats3605"
#define R_OS_LANGUAGE     "en"
#define R_PARENT_ID       "753396045774098432"

enum{
    CID_DISCOVER,
    CID_INFO,
    CID_CONTROL,
    CID_GETSONGLIST,
    CID_GETPLAYLIST,
    CID_QUERYSONGINFO,
    CID_OPERLIST,
    CID_EXIT,
    CID_OPERTUNNEL,
    CID_DEVICEINFO,
};

#define P_DEV_DEVID     "deviceid"
#define P_DEV_HOPE_ID   "hopeid"
#define P_DEV_HOPE_PORT "hopeport"
#define P_DEV_HOPE_IP   "hopeip"
#define P_DEV_V_CODE    "versioncode"
#define P_DEV_V_NAME    "versionname"
#define P_DEV_H_VERSION "haloversion"
#define P_DEV_PLAYER_TYPE   "playertype"
#define P_DEV_MODEL     "model"
#define P_DEV_TARGET_ID "targetid"

#define P_CTRL_EFFECT       "effect"
#define P_CTRL_SOURCE       "source"
#define P_CTRL_VOLUME       "volume"
#define P_CTRL_MODE         "mode"
#define P_CTRL_PLAYSTATE    "playstate"
#define P_CTRL_PROGRESS     "progress"
#define P_CTRL_SONGID       "songid"
#define P_CTRL_CURRSONG     "currsong"

// #define 

#define LIST_MK         "mksonglist"
#define LIST_DEL        "delsonglist"
#define LIST_ADD_SONGS  "addsongs"
#define LIST_RM_SONGS   "removesongs"
#define LIST_SEL        "selectlist"
#define LIST_GET        "getplaylist"

#define MAX_NORMAL_LEN      30
#define MAX_DATA_LEN    128

typedef struct{
    int status;
    int play;
    int ctrl;
    long skip;
    int idvol;
    int mute;
    int setvol;
    int source;
    int effect;
    int model;
    int locale;
}hope_ctrl_t;

typedef struct{
    char    hope_id[MAX_NORMAL_LEN];
    int     hope_port;
    char    hope_ip[MAX_NORMAL_LEN];
    int     version_code;
    char    version_string[MAX_NORMAL_LEN];
    int     halo_version;
    int     player_type;
    char    model[MAX_NORMAL_LEN];
    char    target_id[MAX_NORMAL_LEN];
}pub_dev_info_t;

typedef struct{
    char    album[MAX_DATA_LEN];     //专辑
    char    artist[MAX_DATA_LEN];    //歌手
    char    title[MAX_DATA_LEN];     //歌曲标题
    int     id;         //歌曲id
    int     duration;   //歌曲时长 秒
    boolean_t     favorite;   //是否喜欢
}song_t;

typedef struct{
    char    effect[MAX_NORMAL_LEN];
    char    source[MAX_NORMAL_LEN];
    char    volume[MAX_NORMAL_LEN];
    char    model[MAX_NORMAL_LEN];
    char    playstate[MAX_NORMAL_LEN];
    int     process;
    song_t song;
}pub_info_t;

typedef struct{
    char    deviceid[MAX_NORMAL_LEN];
    boolean_t getinfo;
    char    model[MAX_NORMAL_LEN];
    int     version;
    char    version_name[MAX_NORMAL_LEN];
}sub_heart_t;

typedef struct songs_t{ 
    song_t  song;
    struct songs_t  *next;
}songs_t;

typedef struct{
    char id[MAX_NORMAL_LEN];
    char name[MAX_NORMAL_LEN];
    songs_t songs;
    boolean_t favorite;
    boolean_t isScene;
    boolean_t autoplay;
}play_list_t;

typedef struct{
    int pageindex;
    int pagesize;
    char *playlist_id;
}sub_song_list_t;

typedef struct{
    // char *
}sub_operlist_t;

int set_device_id(char *deviceid);
int get_dev_info(pub_dev_info_t *devinfo);
int get_location(char *province, char *city, char *longitude, char *latitude, char *area);


#endif 