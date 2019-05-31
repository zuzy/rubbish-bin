/*
 * Copyright (C) 2018 Actions
 * Author:lishiyuan
 */
 
#include "common_api.h"
#include "aiengine.h"
#include <json-c/json.h>

typedef struct _engine_info 
{
    int status;
    long int first_time;	
    long start_time;
    long end_time;
    char* tts_url;
} engine_info_t;

static engine_info_t g_enginer_info ={0};

static int (*g_aiengine_start)(struct aiengine *engine, const char *param, char id[64], aiengine_callback callback, const void *usrdata);
static int  (*g_aiengine_stop)(struct aiengine *engine);
static void (*g_tts_callback)(int status, char* tts_url, void* userdata) = NULL;
static void* g_cb_userdata = NULL;


#define DEFAULT_TTS_URL "/tmp/speech_cntts.mp3"

static char* create_cntts_json(char* text, int text_len)
{
    char *buf;	
    char* rbuffer = NULL;
    json_object *object;
    json_object *object1;
    json_object *object2;
    json_object *object3;
	
	
    object = json_object_new_object();
    json_object_object_add(object, "coreProvideType", json_object_new_string("cloud"));

    object1 = json_object_new_object();
    json_object_object_add(object1, "audioType", json_object_new_string("mp3"));
    json_object_object_add(object1, "channel", json_object_new_int(1));
    json_object_object_add(object1, "sampleBytes", json_object_new_int(2));
    json_object_object_add(object1, "sampleRate", json_object_new_int(16000));
    json_object_object_add(object1, "compress", json_object_new_string("raw"));
    json_object_object_add(object, "audio", object1);


    object2 = json_object_new_object();
    json_object_object_add(object2, "userId", json_object_new_string("aispeech"));
    json_object_object_add(object, "app", object2);


    object3 = json_object_new_object();
    json_object_object_add(object3, "coreType", json_object_new_string("cn.sent.syn"));
    json_object_object_add(object3, "res", json_object_new_string("syn_chnsnt_qianranf"));

    json_object_object_add(object3, "refText", json_object_new_string(text));
    json_object_object_add(object3, "speechRate", json_object_new_int(1));
    json_object_object_add(object3, "speechVolume", json_object_new_int(50));
    json_object_object_add(object3, "useStream", json_object_new_int(0));
    json_object_object_add(object, "request", object3);
	
    buf = (char *)json_object_to_json_string_ext(object,JSON_C_TO_STRING_PLAIN);

    rbuffer = strdup(buf);

    json_object_put(object);
    json_object_put(object1);
    json_object_put(object2);
    json_object_put(object3);

    return rbuffer;
}

static long int _get_time() 
{
    long int at = 0;    
    struct timespec now;    
    clock_gettime(CLOCK_MONOTONIC, &now);    
    at = now.tv_sec * 1000 + now.tv_nsec / 1000000;    
    return at; 
}

static int engine_tts_callback(void *usrdata, const char *id, int type, const void *message, int size) 
{
    engine_info_t *info = (engine_info_t *)usrdata; 
    static FILE *audioFd = NULL;	
	
    if (type == AIENGINE_MESSAGE_TYPE_JSON) 
    {        
        printf("[CNTTS]: %.*s\n", size, message);    
    }
    else 
    {//语音合成时，音频数据取出,音频数据以size==0代表音频结尾        
        if (size == 0) 
        {
             info->end_time = _get_time();
		printf("[CNTTS]: end_time-->%dms\n", info->end_time - info->start_time);
		info->status = 2;
		
              if (audioFd)
              {
                  fclose(audioFd);
                  audioFd = NULL;
	        }

              if (g_tts_callback)
                 g_tts_callback(1, info->tts_url, g_cb_userdata);		
	}
	else
	{
           printf("[CNTTS]: size = %d info->status =%d\n", size, info->status);    
	
	    if (info->status == 0) 
	    {
               audioFd = fopen(info->tts_url, "wb+");
               if (audioFd == NULL) 
               {
                   printf("[CNTTS]: open audio file error.\n");
		      return -1;
               }
		
	        info->first_time = _get_time();
	        printf("[CNTTS]: first_time-->%dms\n", info->first_time - info->start_time);
	        info->status = 1; 
	    }

	    if (audioFd)	
	    {
	        if (fwrite(message, 1, size, audioFd) != size)
                   printf("[CNTTS]: write file error.\n");				
	    }	
	}
    }

    return 0;
}


int cntts_set_engine_api(void* start_api, void* stop_api, void* cb_func, void* user_data)
{
    g_aiengine_start = start_api;
    g_aiengine_stop = stop_api;
    g_tts_callback	= cb_func;
    g_cb_userdata = user_data;
    return 0;	
}

int cntts_engine_start(void* engine, char* text, int text_len)	
{
    if (engine == NULL || text == NULL)
        return -1;
  
    int ret = -1;

    char* param = create_cntts_json(text, text_len);

    if (param == NULL)
    {
        return -1;
    }	
    char uuid[64];    
    memset(uuid, 0, sizeof(uuid)); 

    g_enginer_info.tts_url = DEFAULT_TTS_URL;
    g_enginer_info.start_time = _get_time(); 	
    g_enginer_info.status = 0;
		
    if (g_aiengine_start)	
        ret = g_aiengine_start(engine, param, uuid, engine_tts_callback, &g_enginer_info);

    free(param);
	
    if (ret == -1) 
    {
        printf("[CNTTS]: start engine error.\n"); 
        return -1;
    }

    return 0;
}

int cntts_engine_stop(void* engine)
{
    if (engine == NULL)
        return -1;

    int ret = -1;

    if (g_aiengine_stop)	
        ret = g_aiengine_stop(engine);

    if (ret == -1) 
    {
        printf("[CNTTS]: stop engine error.\n"); 
        return -1;
    }
    return 0;
}

