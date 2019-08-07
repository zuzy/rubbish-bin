#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <errno.h>
#include <assert.h>

#include "pipe_cmd.h"

static cmd_msg_t register_cb(char *read, int length);
static cmd_msg_t broadcast_cb(char *read, int length);
static cmd_msg_t target_cb(char *read, int length);
static cmd_msg_t listen_cb(char *read, int length);

struct st_cmd_t{
    char *name;
    int cmd_id;
    cmd_msg_t (*callback) (char *, int);
} _cmd_list[] = {
    {
        .name = "register",
        .cmd_id = CMD_REGIST,
        .callback = register_cb,
    },
    {
        .name = "broadcast",
        .cmd_id = CMD_BROADCAST,
        .callback = broadcast_cb,
    },
    {
        .name = "target",
        .cmd_id = CMD_TARGET,
        .callback = target_cb,
    },
    {
        .name = "listen",
        .cmd_id = CMD_LISTEN,
        .callback = listen_cb,
    },
    {
        .name = NULL,
        .cmd_id = -1,
    }
};

static cmd_msg_t register_cb(char *read, int length) 
{
    assert(read != NULL && length > 0);
    return (cmd_msg_t) {CMD_REGIST, read, 0, NULL};
}

static cmd_msg_t broadcast_cb(char *read, int length) 
{
    assert(read != NULL && length > 0);
    return (cmd_msg_t) {CMD_BROADCAST, NULL, length, read};
}

static cmd_msg_t target_cb(char *read, int length)
{
    assert(read != NULL && length > 0);
    char *body = strchr(read, ':');
    if(body) {
        *body++ = '\0';
        int len = length - (body - read);
        return (cmd_msg_t) {CMD_TARGET, read, len, body};
    } else {
        return (cmd_msg_t) {-1, 0};
    }
}
static cmd_msg_t listen_cb(char *read, int length)
{
    return (cmd_msg_t) {CMD_LISTEN, 0};
}


cmd_msg_t parse_cmd(char *read, int length)
{
    struct st_cmd_t *ptr = _cmd_list;
    while(ptr && ptr->name) {
        int name_size = strlen(ptr->name);
        if(memcmp(read, ptr->name, name_size)) {
            ++ptr;
        }
        // read + name_size + 1, name move tag + ':'
        char *tmp = read + name_size + 1;
        int len = atoi(tmp);
        tmp =  strchr(tmp, ':');
        if(tmp) 
            ++tmp; 
        else 
            break;
        return ptr->callback(tmp, len);
    }
    return (cmd_msg_t){.cmd = -1, 0};
}
