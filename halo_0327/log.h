#ifndef __HALO_LOG__
#define __HALO_LOG__

#define log(format, arg...)   printf("[log]:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);

#endif