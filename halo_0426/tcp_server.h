#ifndef __HALO_TCP_SERVER_H_
#define __HALO_TCP_SERVER_H_

#define GET_STRING_FROM_KEY(json,key) (cJSON_GetObjectItem(json,key)?\
                    cJSON_GetObjectItem(json,key)->valuestring:NULL)

#define GET_INT(json,a) ((json)?((json)->valueint):(a))
#define GET_INT_DEFAULT(json)   GET_INT(json,-1)
#define GET_BOOL(json) ((json)?((json)->type):-1)

void *halo_tcp_proc(void *arg);

#endif