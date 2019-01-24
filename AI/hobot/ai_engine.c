/*
 * Copyright (C) 2017 Actions
 * Author:lishiyuan
 */

#include "common_api.h"
#include "ai_api.h"
#include "hobot_parse.h"

typedef struct
{
    ai_callback ai_event_cb;
    AI_SERVICE_TYPE service_type;
}ai_inner_t;

static ai_inner_t ai_inner ={0};

static void ai_asr_cb(char *pText, int textLen, int errCode)
{
    ai_result_t ai_result;

    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.err_code = errCode;		
		
    if (ai_inner.ai_event_cb)
        ai_inner.ai_event_cb(AI_EVENT_ASR, &ai_result);	
}

static void ai_answer_cb(char *pText, int textLen, char *pUrl[3], int errCode)
{
    ai_result_t ai_result;
    ai_result.text = pText;
    ai_result.text_len = textLen;
    ai_result.urlgroup= pUrl;
		
    ai_result.err_code = errCode;		
		
    if (ai_inner.ai_event_cb)
        ai_inner.ai_event_cb(AI_EVENT_ANSWER, &ai_result);	
}


void* ai_engine_init(void)
{
    int ret = -1;

    #define HOBOT_APPID   "JX-PROD-GEN000"
    #define HOBOT_SECRET "a701e7e7-c059-4c2f-bed0-293c1cb95b1f"

    char sn[1024] = {0};

    if (sys_read_debug_config("SN", sn, sizeof(sn)) != 0)
    {
        if (sair_read_config(sn, sizeof(sn)) != 0)
        {
            printf("read SN failed. \n");
            return NULL;
        }
    }

    ret = hobot_begin(HOBOT_APPID, sn, HOBOT_SECRET, ai_asr_cb, ai_answer_cb);

    if (ret == 0)
    {
        return (void*)&ai_inner;
    }

    return NULL;
}


int ai_engine_feed_data(void* handle, AI_DATA_TYPE data_type, char* data, int data_size)
{
    int ret = -1;
    int tag = 0;
    switch(data_type)
    {
		case AI_DATA_BEGIN:
			tag = POST_DATA_FIRST;
			break;
		case AI_DATA_CONTINUE:
			tag = POST_DATA_CONTINUE;
			break;
		default:
			tag = POST_DATA_END;
			break;
    }
    if (data)
        ret = hobot_post_data(data, data_size>>1, tag, ai_inner.service_type);

    return ret;
}


int ai_engine_finalize(void* handle)
{
    FUNC_ENTER; 

    if (handle == NULL)
       return -1;  

    hobot_end();
	
    FUNC_LEFT; 
    return 0;
}

int ai_engine_interrupt(void* handle)
{
    FUNC_ENTER;

    if (handle == NULL)
       return -1;  	

    hobot_interrupt();
    FUNC_LEFT; 	
    return 0;
}

int ai_engine_get_property(AI_PROPERTY_TYPE property_type, char* value, int length)
{
    if(value == NULL)
    {
        printf("null parameter");
        return -1;		
    }

    memset(value, 0, length);

    if (AIPropertyInitOnce == property_type)
    {
        *value = 0;
    }
	
    return 0;
}

AI_ERROR_CODE ai_engine_set_params(AI_PARAM_TYPE param_type, unsigned long value_size, void *value)
{
    switch(param_type)
    {
        case AI_PARAM_EVENT_CB :
        {
            ai_inner.ai_event_cb = value;	
            break;
        }
        case AI_PARAM_SERVICE_TYPE:
        {
            if (value_size == sizeof(AI_SERVICE_TYPE))
            {
                memcpy(&ai_inner.service_type, value, value_size);
                break;		
            }
            else
            {
                return AI_ERROR_INVALID_PARAM;		
            }
        }		
        default:
            return AI_ERROR_INVALID_PARAM;		

    }
	
    return AI_OK;
}

