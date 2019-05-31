#include <sys/socket.h>
#include <errno.h>
#include "mqttclient.h"
#include "buffer.h"
#include "list.h"
struct st_publish_handle
{
    struct list_head publish_list;
    uint16_t max_id;
};
struct st_publish_handle *publish_handle = NULL;

// LIST_HEAD_INIT()
int publish_init()
{
    REQUIRE((publish_handle = calloc(1, sizeof(struct st_publish_handle))) == NULL, Error);

    publish_handle->publish_list.next = publish_handle->publish_list.prev = &publish_handle->publish_list;
    publish_handle->max_id = 0;

    return 0;
    Error:
        if(publish_handle != NULL) {
            free(publish_handle);
            publish_handle = NULL;
        }
        return -1;
}

int add_pubbody(pub_t *body)
{
    pub_list_t *tmp = NULL;
    REQUIRE(body == NULL, Error);
    tmp = malloc(sizeof(pub_list_t));
    REQUIRE(tmp == NULL, Error);
    tmp->body = body;
    pub_list_t *p, *t;
    list_for_each_entry_safe(p, t, &publish_handle->publish_list, list) {
        REQUIRE(p->body->v_head->id == tmp->body->v_head->id, Error);
        publish_handle->max_id = (publish_handle->max_id > p->body->v_head->id) ? publish_handle->max_id: p->body->v_head->id;
    }
    list_add_tail(&tmp->list, &publish_handle->publish_list);
    return 0;
    Error:
        if(tmp != NULL) {
            free(tmp);
            tmp = NULL;
        }
        return -1;
}

/**
 * @brief  change ctrl type
 *  !! ctrl_type input cannot be PUBLISH !
 * @param  id: 
 * @param  ctrl_type: 
 * @retval 
 */
int mod_publist(uint16_t id, pub_list_t *ptr, uint8_t ctrl_type)
{
    REQUIRE(ctrl_type > PUBCOMP || ctrl_type <= PUBLISH, Error);
    if(ptr == NULL) {
        pub_list_t *p;
        list_for_each_entry(p, &publish_handle->publish_list, list) {
            if( p->body->v_head->id == id) {
                if(p->body->f_head.ctrl_type == ctrl_type) {
                    return 0;
                }
                if(p->body->f_head.ctrl_type == PUBLISH) {
                    p->body->f_head.remain_length = 2;
                    free(p->body->payload);
                    p->body->payload = NULL;
                    free(p->body->v_head->topic);
                    p->body->v_head->topic = NULL;
                    p->body->v_head->top_len = 0;
                }
                p->body->f_head.ctrl_type = ctrl_type;
                return 0;
            }
        }
    } else {
        ptr->body->f_head.ctrl_type = ctrl_type;
        ptr->body->f_head.remain_length = 2;
        if(ptr->body->payload != NULL) {
            free(ptr->body->payload);
            ptr->body->payload = NULL;
        }
        if(ptr->body->v_head->top_len) {
            if(ptr->body->v_head->topic != NULL) {
                free(ptr->body->v_head->topic);
                ptr->body->v_head->topic = NULL;
            }
            ptr->body->v_head->top_len = 0;
        }
    }
    Error:{
        return -1;
    }
}

void dump_pub_v_head(struct st_var_publish_head vhead)
{
    print_mqtt("id %d; len %d; topic: %s", vhead.id, vhead.top_len, vhead.topic);
}

void dump_pub_list()
{
    pub_list_t *p, *t;
    list_for_each_entry_safe(p, t, &publish_handle->publish_list, list) {
        print_mqtt("state %d", p->state);
        dump_fix_head(p->body->f_head);
        dump_pub_v_head(*p->body->v_head);
        dump_payload(*p->body->payload);
    }
}

int delete_pub(uint16_t id)
{
    pub_list_t *p;
    uint16_t m_id = 0;
    list_for_each_entry(p, &publish_handle->publish_list, list) {
        if(p->body->v_head->id == id) {
            list_del(&p->list);
            free(p->body->v_head);
            free(p->body->payload);
            free(p->body);
            free(p);
        } else {
            m_id = (m_id > p->body->v_head->id) ? m_id: p->body->v_head->id;
        }
    }
    publish_handle->max_id = m_id;
    return 0;
}

void free_publist()
{
    pub_list_t *p;
    list_for_each_entry(p, &publish_handle->publish_list, list) {
        list_del(&p->list);
        free(p->body->v_head);
        free(p->body->payload);
        free(p->body);
        free(p);
    }
    publish_handle->max_id = 0;
}

void free_pub(pub_t *p)
{
    if(p != NULL) {
        if(p->payload != NULL) {
            free(p->payload);
        }
        if(p->v_head != NULL) {
            if(p->v_head->topic != NULL) {
                free(p->v_head->topic);
            }
            free(p->v_head);
        }
        free(p);
        p = NULL;
    }
}

pub_t *parse_pub_blob(struct st_data *data)
{
    pub_t *pub = NULL;
    REQUIRE((pub = calloc(1, sizeof(pub_t))) == NULL, Error);
    memcpy(&pub->f_head, &data->f_head, sizeof(data->f_head));
    switch(pub->f_head.ctrl_type) {
        case PUBLISH: {
            int len = get_u16(data->body, data->p_body);
            REQUIRE( (pub->v_head = calloc(1, sizeof(struct st_var_publish_head))) == NULL, Error);
            pub->v_head->top_len = get_u16(data->p_body, data->p_body);
            REQUIRE( (pub->v_head->topic = get_uflow(data->p_body, data->p_body, pub->v_head->top_len)) == NULL, Error);
            pub->v_head->id = get_u16(data->p_body, data->p_body);
            pub->payload->len = pub->f_head.remain_length - len;
            if(pub->payload->len) {
                REQUIRE( (pub->payload->payload = get_uflow(data->p_body, data->p_body, pub->payload->len)) == NULL, Error);
            } else {
                pub->payload->payload = NULL;
            }
            return pub;
            break;
        }
        case PUBACK: 
        case PUBREC: 
        case PUBREL:
        case PUBCOMP: {
            REQUIRE( (pub->v_head = calloc(1, sizeof(struct st_var_publish_head))) == NULL, Error);
            pub->v_head->id = get_u16(data->body, data->p_body);
            return pub;
            break;
        }
        default: {
            break;
        }
    }
    Error: {
        if(pub != NULL) {
            free_pub(pub);
        }
        return NULL;
    }
}

int handle_from_list(pub_t *pub)
{
    pub_t *tmp = NULL;
    switch(pub->f_head.ctrl_type) {
        case PUBLISH: {
            if(pub->f_head.ctrl_type == 0) {
                break;
            }
            REQUIRE((tmp = (pub_t *)calloc(1, sizeof(pub_t))) == NULL, Error);
            tmp->f_head.remain_length = 2;
            REQUIRE((tmp->v_head = calloc(1, sizeof(struct st_var_publish_head))) == NULL, Error);
            tmp->v_head->id = pub->v_head->id;
            if(pub->f_head.ctrl_type == 1) {
                tmp->f_head.ctrl_type = PUBACK;
            }
            if(pub->f_head.ctrl_type == 2) {
                tmp->f_head.ctrl_type = PUBREC;
            }
            add_pubbody(tmp);
            break;
        }
        case PUBCOMP:
        case PUBACK: {
            delete_pub(pub->v_head->id);
            break;
        }
        case PUBREC: 
        case PUBREL: 
        default: {
            mod_publist(pub->v_head->id, NULL, pub->f_head.ctrl_type + 1);
            break;
        }
    }
    return 0;
    Error: {
        if(tmp != NULL) {
            if(tmp->v_head != NULL) {
                free(tmp->v_head);
            }
            free(tmp);
            tmp = NULL;
        }
        return -1;
    }
}

static int _send_pub(int fd, pub_t *pub)
{
    uint8_t *tmp = NULL;
    REQUIRE(fd < 0 || pub == NULL || pub->v_head == NULL, Error);
    char f_head[5] = "";
    uint8_t *ptr = (uint8_t *)f_head;
    uint8_t l_t = 0;
    size_t len = pub->f_head.remain_length;
    *ptr++ = pub->f_head.ctrl_type << 4 | 
                pub->f_head.dup << 3 | 
                pub->f_head.qos << 2 |
                pub->f_head.retain;
    
    for(l_t = 1; len > 0; len >>= 7, ++l_t) {
        *ptr++ = len & 0x7f;
    }
    REQUIRE((tmp = calloc(1, pub->f_head.remain_length + l_t)) == NULL, Error);
    ptr = tmp;
    memcpy(tmp, f_head, l_t);
    ptr += l_t;

    if(pub->v_head->top_len) {
        *ptr++ = pub->v_head->top_len >> 8;
        *ptr++ = pub->v_head->top_len & 0xff;
        memcpy(ptr, pub->v_head->topic, pub->v_head->top_len);
        ptr += pub->v_head->top_len;
    }
    *ptr++ = pub->v_head->id >> 8;
    *ptr++ = pub->v_head->id & 0xff;

    len = pub->f_head.remain_length - 2;
    len -= pub->v_head->top_len ? pub->v_head->top_len + 4: 2;

    if(len && pub->payload != NULL) {
        memcpy(ptr, pub->payload, len);
        ptr += len;
    }

    Retry:{
        len = send(fd, tmp, ptr-tmp, 0);
        if(len != ptr - tmp) {
            REQUIRE(errno == EINTR, Retry);
            goto Error;
        }
    }
    free(tmp);
    tmp = NULL;
    return 0;
    
    Error: {
        if(tmp != NULL) {
            free(tmp);
            tmp = NULL;
        }
        return -1;
    }
}

int handle_publist(int fd)
{
    if(fd < 0) {
        return -1;
    }
    pub_list_t *p;
    list_for_each_entry(p, &publish_handle->publish_list, list) {
        if(_send_pub(fd, p->body) == 0) {
            switch(p->body->f_head.ctrl_type) {
                case PUBLISH: {
                    if(p->body->f_head.qos) {
                        break;
                    }
                }
                case PUBACK:
                case PUBCOMP: {
                    free_pub(p->body);
                    list_del(&p->list);
                    break;
                }
                case PUBREC:
                case PUBREL:
                default: {
                    break;
                }
            }
        }
    }
}
