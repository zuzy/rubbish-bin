#ifndef __HALO_HTTP_REGIST_H__
#define __HALO_HTTP_REGIST_H__

#include "common.h"
#include "halo_http.h"

#define HALO_REGIST_DEBUG 1
#if HALO_REGIST_DEBUG == 1
    #define print_regist(format, arg...)   do { printf("\033[36m[halo_regist]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_regist(format, arg...)   do {} while (0)
#endif

#define DAT_COMNAME         "comName"
#define DAT_DEV_SN          "deviceSN"
#define DAT_DEV_NAME        "deviceName"
#define DAT_DEV_CATA        "deviceCata"
#define DAT_VER_FIRM        "firmVersion"
#define DAT_VER_SOFT        "softVersion"
// #define DAT_VER_SOFT        "softVersi&n"
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

#define MAX_ID_LEN  100

int regist_mac();
int regist();
int regist_first();
int regist_second();
int regist_property_dy(st_post *post);
int regist_join_dy(st_post *post);
#endif