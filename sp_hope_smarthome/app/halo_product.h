#ifndef __HALO_PRODUCT_H__
#define __HALO_PRODUCT_H__

#define HALO_PRODUCT_DEBUG 1
#if HALO_PRODUCT_DEBUG
    #define print_product(format, arg...)   do { printf("\033[31m[halo_product]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} \
                                        while (0)
#else
    #define print_product(format, arg...)   NULL
#endif

#define HALO_VER    "1.0"
#define HALO_CID    "750832243333435392"
#define HALO_SID    "750837261197414400"
#define HALO_KEY    "306BE98143724C6481E5F2BB7F6951D5"
#define HALO_SN     "74001805110012"

int get_mac_string(char *mac);
int get_server_ip(char *ip);
int get_dev_sn(char *sn);
int get_dev_uuid(char *uuid);
#endif