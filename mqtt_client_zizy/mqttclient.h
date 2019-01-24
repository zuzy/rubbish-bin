#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "list.h"


#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_mqtt(format, arg...)   do { printf("\033[31m[mqtt]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_mqtt(format, arg...)   NULL
#endif

// #define print_log(format, arg...)   do{ syslog(LOG_INFO, "[halotest log] %s,%d -> "format"\n",__FILE__,__LINE__, ## arg);}while(0)
// #define REQUIRE(in,tag)                 do{if(in){print_log("%s %s", #tag, #in); goto tag;}}while(0)
#define REQUIRE(in,tag)                 do{if(in){print_mqtt("%s %s", #tag, #in); goto tag;}}while(0)
#define REQUIRE_NOLOG(in,tag)           do{if(in){print_mqtt("%s %s", #tag, #in); goto tag;}}while(0)


enum en_ctrl_type
{
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK = 4,
    PUBREC = 5,
    PUBREL = 6,
    PUBCOMP = 7,
    SUBSCRIBE = 8,
    SUBACK = 9,
    UNSUBSCRIBE = 10,
    UNSUBACK = 11,
    PINTREQ = 12,
    PINGRESP = 13,
    DISCONNECT = 14,
};

enum en_connstate
{
    TCP_CONNOK,
    M_CONNING,
    M_CONNED,
    M_SUBING,
    M_PUBING,
    M_CLOSE,
};

typedef struct st_fix_head
{
    uint8_t     ctrl_type:4;
    uint8_t     dup:1;
    uint8_t     qos:2;
    uint8_t     retain:1;
    uint32_t    remain_length;
}fix_head;

typedef struct st_payload
{
    uint32_t len;
    uint8_t *payload;
}payload_t;

typedef struct st_var_connect_head
{
    char *      pro_name;
    uint8_t     level;
    uint8_t     username_flag:1;
    uint8_t     password_flag:1;
    uint8_t     will_retain:1;
    uint8_t     will_qos:2;
    uint8_t     will_flag:1;
    uint8_t     clean_session:1;
    uint8_t     reserved:1;
    uint16_t    keep_alive;
}var_head;

struct st_var_publish_head
{
    uint16_t    top_len;
    uint8_t     *topic;
    uint16_t    id;
};

enum en_pub_state
{
    act_pubing = 0x00,
    act_pub_ready = 0x01,
    act_pub_wait_ack = 0x02,
    act_pub_wait_rec = 0x03,
    act_pub_wait_cmp = 0x04,

    pas_pub = 0x08,
    pas_pub_wait_rel = 0x09,
    pub_finish = 0x0f,
};

typedef struct st_pub
{
    fix_head            f_head;
    struct st_var_publish_head  *v_head;
    payload_t           *payload;
}pub_t;

typedef struct st_pub_list
{
    struct list_head    list;
    pub_t               *body;
    uint8_t             state;
}pub_list_t;

struct st_mqtt_handle;

uint8_t get_u8(uint8_t *body, uint8_t *ptr);
uint16_t get_u16(uint8_t *body, uint8_t *ptr);
uint8_t *get_uflow(uint8_t *body, uint8_t *ptr, size_t len);
int get_fix_header(uint8_t *body, fix_head *head, uint8_t **ptr);
void dump_fix_head(fix_head head);
void dump_payload(payload_t p);

extern struct st_mqtt_handle mqtt_handle;

#endif