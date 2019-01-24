#ifndef __TCP_SER_CLI_H__
#define __TCP_SER_CLI_H__

#define HALO_TCP_SER_CLI_DEBUG 0
#if HALO_TCP_SER_CLI_DEBUG == 1
    #define print_ser_cli(format, arg...)   do { printf("\033[31m[halo_ser_cli]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_ser_cli(format, arg...)   NULL
#endif

int add_tcp_ser_cli(int fd, char *name);

/** 
 * @brief  get the number of clients online
 * @note   just a status
 * @retval the number
 */
int tsc_active_num();

/** 
 * @brief  add msg to list, call send activly
 * @note   type & content, send the content
 * @param  type: type is still useless now
 * @param  *content: the body to send
 * @retval the result of addition to msg list
 */
int msg_to_ser_cli(int type, char *content);

#endif