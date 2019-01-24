#ifndef __HTTP_REGIST_H__
#define __HTTP_REGIST_H__

// #define POST_VER            "ver"
// #define POST_DES            "des"
// #define POST_CID            "cid"
// #define POST_SID            "sid"
// #define POST_KEY            "key"
// #define POST_LEN            "len"
// #define POST_DAT            "dat"

#define REGIST_DEV_URI      "http://192.168.2.9:8080/hopeApi/device/register"
// #define REGIST_DEV_URI      "http://192.168.2.7:80/hopeApi/device/register"

#define REGIST_PRO_URI      "http://192.168.2.9:8080/hopeApi/device/status"

#define DAT_COMNAME         "comName"

#define DAT_DEV_SN          "deviceSN"
#define DAT_DEV_NAME        "deviceName"
#define DAT_DEV_CATA        "deviceCata"

#define DAT_VER_FIRM        "firmVersion"
#define DAT_VER_SOFT        "softVersion"
#define DAT_MAC             "macAddress"

#define DAT_PLY_TYPE        "playerType"
#define DAT_PLY_VER         "playerVersion"

#define DAT_BG_TYPE         "bgType"
#define DAT_BG_VER          "bgVersion"

#define DAT_UPGRADE_CH      "upgradeChannel"
#define DAT_DEV_DRIVE       "deviceDrive"
#define DAT_OS_LANG         "osLanguage"
#define DAT_PARENT          "parentId"

#define DAT_LOC_LONG        "deviceLong"
#define DAT_LOC_LAT         "deviceLat"
#define DAT_LOC_PROVINCE    "provinceCode"
#define DAT_LOC_CITY        "cityCode"
#define DAT_LOC_AREA        "areaCode"
#define DAT_LOC_ADDRESS     "deviceAddress"

#define RET_CODE            "code"
#define RET_MSG             "message"
#define RET_OBJ             "object"
#define RET_OBJ_AUTH        "authCode"
#define RET_OBJ_REF         "refrenceId"

typedef struct{
    char auth[32];
    char ref[18];
}regist_t;

int regist_device(char *auth, unsigned char *ref);
int regist_property();
#endif