#ifndef __VILLA_H__
#define __VILLA_H__

#define HALO_VILLA_DEBUG 1
#if HALO_VILLA_DEBUG == 1
    #define print_villa(format, arg...)   do { printf("\033[31m[halo_villa]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_villa(format, arg...)   NULL
#endif

#include "common.h"



int add_villa_pro();
#endif