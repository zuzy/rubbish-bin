/*
 * Copyright (C) 2017 Actions
 * Author:lishiyuan
 */
#include "common_api.h"
#include "ai_api.h"

void* ai_engine_init(void)
{
    return NULL;
}

int ai_engine_finalize(void* handle)
{
    return -1;
}

int ai_engine_feed_data(void* handle, AI_DATA_TYPE data_type, char* data, int data_size)
{
    return -1;
}

int ai_engine_interrupt(void* handle)
{
    return -1;
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
    return -1;
}



