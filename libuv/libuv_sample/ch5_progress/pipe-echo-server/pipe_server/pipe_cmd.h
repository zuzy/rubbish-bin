#ifndef __ZIZY_PIPE_CMD_H__
#define __ZIZY_PIPE_CMD_H__

enum en_cmd_id
{
    CMD_INIT = 0,
    CMD_REGIST,
    CMD_BROADCAST,
    CMD_TARGET,
    CMD_LISTEN,
    CMD_FINAL,
};

typedef struct _st_cmd_msg_t {
    int cmd;
    char *target;
    int length;
    char *body;
}cmd_msg_t;

cmd_msg_t parse_cmd(char *read, int length);

#endif