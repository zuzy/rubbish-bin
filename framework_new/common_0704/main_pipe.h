
#ifndef __MAIN_PIPE_H__
#define __MAIN_PIPE_H__

#define HALO_PIPE_DEBUG 0
#if HALO_PIPE_DEBUG == 1
    #define print_pipe(format, arg...)   do { printf("\033[31m[halo_pipe]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_pipe(format, arg...)   NULL
#endif


#define MAX_PIPE_PAYLOAD_LEN 4096

int add_pipe();
int update_pipe(void *arg);
#endif