#ifndef __HOPE_MSG_H__
#define __HOPE_MSG_H__

#include <applib.h>

typedef enum
{
	HOPE_PLAYER_STATUS_UPDATE = MSG_SRV_USER_DEFINE + 20000,
	HOPE_DEV_UNBIND,
	HOPE_GET_SPEECH,
    HOPE_TP_DEV_CTRL,
} hope_msg_type_e;

#endif