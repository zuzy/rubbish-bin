#include <stdint.h> 
#include <string.h>
#include "act_debug.h"
#include "applib.h"
#include "hope_msg.h"
#include "smart_player_api.h"
#include "smart_player_hal.h"
#include "case_msg.h"

int32_t hope_player_status_update(splayer_hal_update_t *update)
{
    return 0;
    if(get_app_pid("halotest") == -1){
        print_zizy("halotest is not working");
        return 0;
    }
    int ret =  0;
    srv_msg_t cmd;

    cmd.type = HOPE_PLAYER_STATUS_UPDATE;
    // cmd.type = MSG_HOPE_STATUS_UPDATE;
    cmd.sync = 0;
    cmd.c_size = sizeof(splayer_hal_update_t);
    memcpy(cmd.content, (void*)update, sizeof(splayer_hal_update_t));

    ret = send_service_cmd(APP_NAME_HALOTEST, &cmd, NULL);
    // if(ret < 0){
    //     halotest_status = 0;
    // }
    return ret;
    // return send_async_msg(APP_NAME_HALOTEST, &cmd);
}

int32_t hope_dev_unbind(void)
{
    set_config(CFG_HREG_MAC, "ffff", 32);
    if(get_app_pid("halotest") == -1){
        print_zizy("halotest is not working");
        return 0;
    }
    srv_msg_t cmd;
	srv_msg_t reply;
    int32_t ret = 0;
	
	cmd.type = HOPE_DEV_UNBIND;
    cmd.sync = 1;
    cmd.c_size = 0;
    ret = send_service_cmd(APP_NAME_HALOTEST, &cmd, &reply);

    if((ret < 0)
        || (reply.type != MSG_REPLY_SUCCESS))
    {
        print_err("hope device unbind failed:%d %d fail\n", ret, reply.type);
        return -1;
    }	
	return ret;	
}

int32_t hope_tp_dev_ctrl(int id, int status)
{
    print_zizy("hope third party device control %d %d", id , status);
    srv_msg_t cmd, reply;
    int ret = 0;
	cmd.type = HOPE_TP_DEV_CTRL;
    cmd.sync = 1;
    cmd.c_size = 2 * sizeof(int);
    memcpy(cmd.content, (void*)&id, sizeof(int));
    memcpy(cmd.content + sizeof(int), (void *)&status, sizeof(int));
    ret = send_service_cmd(APP_NAME_HALOTEST, &cmd, &reply);
}

int32_t hope_speech_cmd(char *content)
{
    if(get_app_pid("halotest") == -1){
        print_zizy("halotest is not working");
        return 0;
    }
    srv_msg_t cmd;
	srv_msg_t reply;
    int32_t ret = 0;
	
	cmd.type = HOPE_GET_SPEECH;
    cmd.sync = 1;
    cmd.c_size = strlen(content);
    strncpy(cmd.content, content, 1024);
    ret = send_service_cmd(APP_NAME_HALOTEST, &cmd, &reply);

    if((ret < 0)
        || (reply.type != MSG_REPLY_SUCCESS))
    {
        print_err("hope device unbind failed:%d %d fail\n", ret, reply.type);
        return -1;
    }	
	return ret;	
}