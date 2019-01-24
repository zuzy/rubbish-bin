#include "mqttclient.h"
#include "buffer.h"
#include "buff_parse.h"

#define BUFF_LIST       X(connect) \
                        X(connack) \
                        X(publish) \
                        X(puback) \
                        X(pubrec) \
                        X(pubrel) \
                        X(pubcomp) \
                        X(subscribe) \
                        X(suback) \
                        X(unsubscribe) \
                        X(unsuback) \
                        X(pingreq) \
                        X(pingresp) \
                        X(disconnect)


int connect_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;
    print_mqtt();

    return 0;
    Error:{
        return -1;
    }
}

int connack_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;
    print_mqtt();
    return 0;
    Error:{
        return -1;
    }
}

int publish_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int puback_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int pubrec_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int pubrel_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int pubcomp_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int subscribe_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int suback_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int unsubscribe_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int unsuback_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int pingreq_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int pingresp_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

int disconnect_cb(void *arg)
{
    struct st_data *data = (struct st_data *)arg;

    return 0;
    Error:{
        return -1;
    }
}

enum
{
    CTRL_NULL = 0,
    #define X(a,...) e_##a,
    BUFF_LIST
    #undef X
};

struct {
    const char  *name;
    uint8_t     ctrltype;
    int         (*callback)(void *);
} parse_list[] = {
    #define X(n) {#n, e_##n, n##_cb},
    BUFF_LIST
    #undef X

    {"", 0, NULL}
};

int parse_buf(struct st_buffer *buf)
{
    typeof(&parse_list[0]) ptr = parse_list;
    while(ptr != NULL && ptr->ctrltype) {
        if(ptr->ctrltype == buf->data.f_head.ctrl_type) {
            return ptr->callback(&buf->data);
        }
        ++ptr;
    }
}