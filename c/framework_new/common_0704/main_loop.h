#ifndef __MAIN_LOOP_H__
#define __MAIN_LOOP_H__

#define HALO_MAIN_DEBUG 1
#if HALO_MAIN_DEBUG == 1
    #define print_main(format, arg...)   do { printf("\033[31m[halo_main]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_main(format, arg...)   NULL
#endif

#include "common.h"

extern st_halo *halo_handle;


int halo_init(callback hal_cb);
int halo_start();
int update_status(st_hal hal);
#endif