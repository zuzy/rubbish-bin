#ifndef __MQTT_BUFFER_H__
#define __MQTT_BUFFER_H__
#include <stdint.h>
#include "mqttclient.h"
#define MAX_BUFFER_LEN  1024

struct st_data
{
    fix_head    f_head;
    uint8_t     *body;
    uint8_t     *p_body;
};

struct st_buffer
{
    uint8_t         buffer[MAX_BUFFER_LEN];
    uint8_t         *p_buf;
    int             len;
    struct st_data  data;
    uint32_t        data_left;
    uint8_t         finish;
};

void clearbuf(struct st_buffer *buf);
void mvforward(char *des, int size, int step);
int checkbuf(struct st_buffer *buf);
int get_body(struct st_buffer *buf, fix_head *f_head, uint8_t *body);
int transform(struct st_buffer *buf);
void resetbody(struct st_buffer *buf);
#endif