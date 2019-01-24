#include "mqttclient.h"
#include "buffer.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

void clearbuf(struct st_buffer *buf)
{
    if(buf->data.body != NULL) {
        free(buf->data.body);
    }
    memset(buf, 0, sizeof(*buf));
}

void resetbody(struct st_buffer *buf)
{
    if(buf->data.body != NULL) {
        free(buf->data.body);
    }
    memset(&buf->data, 0, sizeof(buf->data));
    buf->data_left = 0;
    buf->finish = 0;
}

void mvforward(char *des, int size, int step)
{
    if(step > size)
        return;
    int len = size - step;
    char *ptr = des;
    int i = 0;
    for(; i < len; i++) {
        *ptr = *(ptr + step);
        ++ptr;
    }
    memset(des + size - step, 0, step);
}

static int _mvf_buf(struct st_buffer *buf, uint16_t step)
{
    if(step > buf->len) {
        return -1;
    }
    mvforward((char *)buf->buffer, buf->len, step);
    buf->len -= step;
    buf->p_buf -= step;
    return 0;
}

int checkbuf(struct st_buffer *buf)
{
    return buf->finish;
}

int get_body(struct st_buffer *buf, fix_head *f_head, uint8_t *body)
{
    if(checkbuf(buf)) {
        f_head = &buf->data.f_head;
        body = buf->data.body;
        return 0;
    }
    Error: {
        body = f_head = NULL;
        return -1;
    }
}

int transform(struct st_buffer *buf)
{
    if(buf->finish) {
        return 0;
    }
    if(buf->data_left > 0) {
        uint32_t len = buf->len < buf->data_left? buf->len: buf->data_left;
        memcpy(buf->data.body, buf->buffer, len);
        buf->data.p_body += len;
        _mvf_buf(buf, len);
        buf->data_left -= len;
        if(!buf->data_left) {
            buf->finish = 1;
        }
        return 0;

    } else {
        int l = get_fix_header(buf->buffer, &buf->data.f_head, &buf->p_buf);
        _mvf_buf(buf, l);

        buf->data_left = buf->data.f_head.remain_length;
        REQUIRE((buf->data.body = calloc(1, buf->data.f_head.remain_length)) == NULL, Error);
        buf->data.p_body = buf->data.body;
        return transform(buf);
    }
    Error:
        return -1;
}

